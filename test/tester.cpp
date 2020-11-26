// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

struct test_cfg {
  std::string description;
  std::string name;
  fs::path path;
  std::string command = "dwt";
  bool skip = false;
  int loops = 1;
};

struct kv_pair {
  std::string key;
  std::string value;
};

namespace {

std::atomic<int> total_iterations = 0;
std::atomic<int> total_tests = 0;
std::atomic<int> total_tests_passed = 0;
std::atomic<int> total_tests_failed = 0;
std::atomic<int> total_tests_skipped = 0;

std::mutex loglock;
std::ofstream logfile;

bool interactive = false;

} // namespace

kv_pair parse_kv_pair(FILE *fd) {
  bool parsing_key = true;
  bool parsing_str = false;
  uint8_t ch = 0;
  kv_pair kvp;

  while (fread(&ch, 1, 1, fd)) {
    switch (ch) {
    case '\n':
      if (!parsing_key && !parsing_str) {
        return kvp;
      }
      [[fallthrough]];
    case '\r':
    case '\t':
    case ' ':
      if (parsing_str) {
        assert(parsing_key == false);
        kvp.value += ch;
      }
      break;
    case '"':
      if (parsing_key) {
        throw std::runtime_error("syntax error: unexpected '\"'");
      } else if (parsing_str) {
        parsing_str = false;
      } else {
        parsing_str = true;
      }
      break;
    case ':':
      if (parsing_key) {
        if (kvp.key == "") {
          throw std::runtime_error("syntax error: missing key");
        }
        parsing_key = false; // now parsing value
      } else {
        throw std::runtime_error("syntax error: unexpected ':'");
      }
      break;
    default:
      if (parsing_key) {
        kvp.key += ch;
      } else {
        kvp.value += ch;
      }
      break;
    }
  }

  return kvp;
}

std::vector<kv_pair> parse_kv_pairs(fs::path &cfg) {
  std::vector<kv_pair> kv_pairs;

  FILE *fd = fopen(cfg.string().c_str(), "rb");
  if (!fd) {
    return kv_pairs;
  }

  try {
    while (1) {
      auto kvp = parse_kv_pair(fd);
      if (kvp.key != "") {
        kv_pairs.push_back(kvp);
      } else {
        break;
      }
    }
  } catch (...) {
  }

  fclose(fd);

  return kv_pairs;
}

void get_description(test_cfg &test, std::vector<kv_pair> &kv_pairs) {
  for (auto &kvp : kv_pairs) {
    if (kvp.key == "description") {
      test.description = kvp.value;
      break;
    }
  }
}

void get_name(test_cfg &test, std::vector<kv_pair> &kv_pairs) {
  for (auto &kvp : kv_pairs) {
    if (kvp.key == "name") {
      test.name = kvp.value;
      break;
    }
  }
}

void get_command(test_cfg &test, std::vector<kv_pair> &kv_pairs) {
  for (auto &kvp : kv_pairs) {
    if (kvp.key == "command") {
      test.command = kvp.value;
      break;
    }
  }
}

void get_skip(test_cfg &test, std::vector<kv_pair> &kv_pairs) {
  for (auto &kvp : kv_pairs) {
    if (kvp.key == "skip") {
      if (kvp.value == "yes") {
        test.skip = true;
      } else if (kvp.value != "no") {
        throw std::runtime_error("parse error: expected \"yes\" or \"no\"");
      }
      break;
    }
  }
}

void get_loops(test_cfg &test, std::vector<kv_pair> &kv_pairs) {
  for (auto &kvp : kv_pairs) {
    if (kvp.key == "loops") {
      test.loops = atoi(kvp.value.c_str());
      break;
    }
  }
}

std::shared_ptr<test_cfg> parse_test(fs::path &cfg) {
  auto test = std::make_shared<test_cfg>();
  auto pairs = parse_kv_pairs(cfg);

  get_description(*test, pairs);
  get_name(*test, pairs);
  test->path = cfg.parent_path();
  get_command(*test, pairs);
  get_skip(*test, pairs);
  get_loops(*test, pairs);

  if (test->skip == false) {
    total_iterations += test->loops;
    total_tests += 1;
  }

  return test;
}

std::vector<fs::path> find_tests() {
  std::vector<fs::path> paths;

  for (auto &p : fs::recursive_directory_iterator(".")) {
    if (p.path().extension() == ".cfg") {
      paths.push_back(p.path());
    }
  }

  return paths;
}

bool files_match(std::string file0, std::string file1) {
  bool match = true;

  do {
    FILE *fd0 = fopen(file0.c_str(), "rb");

    if (!fd0) {
      match = false;
      break;
    }

    FILE *fd1 = fopen(file1.c_str(), "rb");

    if (!fd1) {
      match = false;
      fclose(fd0);
      break;
    }

    uint8_t buf0[1024];
    uint8_t buf1[1024];

    do {
      size_t s0 = fread(buf0, 1, 1024, fd0);
      size_t s1 = fread(buf1, 1, 1024, fd1);

      if (s0 != s1 && s0 > 0) {
        match = false;
        break;
      }

      if (memcmp(buf0, buf1, s0)) {
        match = false;
        break;
      }
    } while (!feof(fd0) || !feof(fd1));

    fclose(fd0);
    fclose(fd1);
  } while (0);

  return match;
}

enum test_status { TEST_PASSED, TEST_FAILED, TEST_SKIPPED };

void log(std::string txt, bool echo = false) {
  std::scoped_lock hold(loglock);

  if (echo) {
    printf("%s", txt.c_str());
    printf("\n");
  }

  if (logfile.is_open()) {
    logfile << txt << "\n";
  }
}

void log_test(test_cfg &cfg, test_status status, unsigned int ms) {
  switch (status) {
  case TEST_PASSED:
    log("  [PASS]  " + cfg.name + "; " + std::to_string(ms) + "ms");
    total_tests_passed += 1;
    break;
  case TEST_FAILED:
    log("  [FAIL]  " + cfg.name + "; " + std::to_string(ms) + "ms");
    total_tests_failed += 1;
    break;
  case TEST_SKIPPED:
    log("  [SKIP]  " + cfg.name);
    total_tests_skipped += 1;
    break;
  }
}

void stats() {
  std::scoped_lock hold(loglock);
  int completed = total_tests_passed + total_tests_failed + total_tests_skipped;
  int total = total_tests;

  printf("\r%d/%d", completed, total);
  fflush(stdout);
}

void run_test(test_cfg test) {
  std::string test_base = test.path.string() + "/" + test.name;
  std::string out_file = test_base + ".stdout";
  std::string err_file = test_base + ".stderr";
  std::string cmd_line = test.command + " " + test_base + ".dwt ";
  cmd_line += "> " + out_file + " 2> " + err_file;

  do {
    auto start = std::chrono::system_clock::now();
    int status = system(cmd_line.c_str());
    auto end = std::chrono::system_clock::now();
    auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (status != 0) {
      log_test(test, TEST_FAILED, elapsed.count());
      break;
    }

    if (fs::exists(test_base + ".err")) {
      if (!files_match(test_base + ".err", err_file)) {
        log_test(test, TEST_FAILED, elapsed.count());
        break;
      }
    } else if (fs::exists(test_base + ".out")) {
      if (!files_match(test_base + ".out", out_file)) {
        log_test(test, TEST_FAILED, elapsed.count());
        break;
      }
    }

    log_test(test, TEST_PASSED, elapsed.count());
  } while (0);

  if (interactive) {
    stats();
  }
}

class thread_pool {
public:
  thread_pool(std::vector<std::shared_ptr<test_cfg>> tests)
    : _tests(tests) {
    unsigned nr_threads = std::thread::hardware_concurrency();

    if (nr_threads == 0) {
      nr_threads = 1;
    }

    log("Threads: " + std::to_string(nr_threads), true);
    log("");

    for (unsigned i = 0; i < nr_threads; ++i) {
      _threads.push_back(std::thread(&thread_pool::run, this));
    }
  }

  ~thread_pool() {
  }

  void join() {
    for (auto &thread : _threads) {
      thread.join();
    }
  }

private:
  std::shared_ptr<test_cfg> next_test() {
    std::scoped_lock hold(_mutex);
    std::shared_ptr<test_cfg> cfg;
    if (_tests.size() > 0) {
      cfg = _tests.back();
      _tests.pop_back();
    }
    return cfg;
  }

  void run() {
    std::shared_ptr<test_cfg> test;

    while ((test = next_test())) {
      run_test(*test);
    }
  }

  std::vector<std::shared_ptr<test_cfg>> _tests;
  std::vector<std::thread> _threads;
  std::mutex _mutex;
};

void open_log() {
  logfile.open("log.txt");
}

void close_log(unsigned int secs) {
  log("Skipped: " + std::to_string(total_tests_skipped.load()), true);
  log(" Passed: " + std::to_string(total_tests_passed.load()), true);
  log(" Failed: " + std::to_string(total_tests_failed.load()), true);
  log("Took " + std::to_string(secs) + " seconds", true);

  if (logfile.is_open()) {
    logfile.close();
  }
}

void banner() {
  system("dwt version.dwt > version.txt");
  std::ifstream ifs;
  ifs.open("version.txt");
  std::stringstream ss;
  ss << "Dwt " << ifs.rdbuf();
  ifs.close();
  fs::remove("version.txt");

  std::time_t now =
    std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::string datestr(32, '\0');
  std::strftime(
    &datestr[0], datestr.size(), "%d/%m/%Y %H:%M:%S", std::localtime(&now));
  ss << datestr << "\n";

  log(ss.str().c_str(), true);
}

void generate_fuzz_test(test_cfg &test, unsigned int fuzz_tc_num) {
  std::string fuzz_tc_name =
    "fuzz_tc_" + std::to_string(fuzz_tc_num) + "_" + test.name;

  std::string fuzz_base = "fuzz/" + fuzz_tc_name + "/" + fuzz_tc_name;
  std::string orig_base = test.path.string() + "/" + test.name;

  fs::create_directory("fuzz/" + fuzz_tc_name);

  std::ofstream ofs;
  ofs.open(std::string(fuzz_base + ".cfg").c_str());
  ofs << "name: " << fuzz_tc_name.c_str() << "\n";
  ofs << "command: " << test.command.c_str() << "\n";

  std::string fuzzer_cmd =
    "./fuzzer " + orig_base + ".dwt" + " > " + fuzz_base + ".dwt";

  system(fuzzer_cmd.c_str());
}

bool generate_fuzz_tests(std::vector<std::shared_ptr<test_cfg>> &tests) {
  if (!fs::exists("fuzz")) {
    fs::create_directory("fuzz");
  } else {
    return false;
  }

  printf("Generating fuzz tests\n");

  unsigned int fuzz_tc_num = 0;

  for (auto &test : tests) {
    for (int i = 0; i < 100; ++i) {
      generate_fuzz_test(*test, ++fuzz_tc_num);
      if (interactive) {
        printf("\r%d/%ld", fuzz_tc_num, tests.size() * 100);
        fflush(stdout);
      }
    }
  }

  printf("\n");

  return true;
}

int main(int argc, char **argv) {
  if (argc > 1) {
    if (strcmp(argv[1], "-i") == 0) {
      interactive = true;
    }
  }

  std::vector<std::shared_ptr<test_cfg>> tests;

  for (auto &test : find_tests()) {
    tests.push_back(parse_test(test));
  }

  open_log();
  banner();

  if (generate_fuzz_tests(tests)) {
    tests.clear();
    total_iterations = 0;
    total_tests = 0;

    for (auto &test : find_tests()) {
      tests.push_back(parse_test(test));
    }
  }

  printf("Running tests\n");

  auto start = std::chrono::system_clock::now();
  thread_pool pool(tests);
  pool.join();
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
  float tests_per_sec = (float) total_tests / elapsed.count();

  printf("\n%.1f tps\n", tests_per_sec);

  if (logfile.is_open()) {
    logfile << "\n---\n" << tests_per_sec << " tests/sec\n";
  }

  close_log(elapsed.count());

  if (total_tests_passed + total_tests_skipped == total_tests) {
    return 0;
  } else {
    return 1;
  }
}
