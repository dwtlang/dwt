#!/usr/bin/env python3

import subprocess
import signal
import yaml
import re
import os
import sys
import datetime

stop_on_error = False
use_colorama = False
use_tqdm = False
junit_file_path = "dwt_junit.xml"
junit_file = open(junit_file_path, "w")
total_tests = 0
test_category = ""
tc_repeat = 1

try:
    from colorama import Fore
    from colorama import Style
    use_colorama = True
except:
    print("install colorama for coloured output")
try:
    from tqdm import tqdm
    use_tqdm = True
except:
    print("install tqdm to see a progress bar")
import time

if len(sys.argv) > 1:
    for arg in sys.argv[1:]:
        if arg == "--jenkins":
            use_tqdm = False
        elif arg == "--stop-on-error":
            stop_on_error = True
        else:
            test_category = arg

out = None

top = os.getcwd()
tclist = []

pass_count = 0
fail_count = 0
segv_count = 0
skip_count = 0

cmd = "dwt"

def say(message):
    if out is None:
        print(message)
    else:
        pass #out.write(message)

def esc_seq(txt):
    esc_txt = ""
    for c in txt:
        if c == "&":
            esc_txt += "&amp;"
        elif c == "<":
            esc_txt += "&lt;"
        elif c == ">":
            esc_txt += "&gt;"
        elif c == "\"":
            esc_txt += "&quot;"
        elif c == "\'":
            esc_txt += "&apos;"
        else:
            esc_txt += c
    return esc_txt

def calc_percentage_complete():
    pc_complete = int(((pass_count + fail_count + skip_count) / total_tests) * 100)

    pc_complete_str = ""
    if pc_complete < 10:
        pc_complete_str = "  " + str(pc_complete) + "%"
    elif pc_complete < 100:
        pc_complete_str = " " + str(pc_complete) + "%"
    else:
        pc_complete_str = str(pc_complete) + "%"

    if use_colorama == True:
        if fail_count > 0:
            return f"{Fore.RED}[" + pc_complete_str + f"]{Style.RESET_ALL}"
        else:
            return f"{Fore.GREEN}[" + pc_complete_str + f"]{Style.RESET_ALL}"
    else:
        return "[" + pc_complete_str + "]"

def tc_fail(tcname, proc, elapsed):
    global fail_count
    fail_count += 1
    pcmpl = calc_percentage_complete()
    if use_colorama:
        say(f"  {pcmpl}  {Fore.RED}[FAIL]{Style.RESET_ALL}  {tcname} ({elapsed}s)")
    else:
        say(f"  {pcmpl}  [FAIL]  {tcname} ({elapsed}s)")
    junit_file.write(f"<testcase name=\"{tcname}\" time=\"{elapsed}\">\n")
    if proc != None:
        junit_file.write(f"<failure message=\"\" type=\"\"/>\n")
        junit_file.write(f"<system-out>\n{esc_seq(proc.stdout)}</system-out>\n")
        junit_file.write(f"<system-err>\n{esc_seq(proc.stderr)}</system-err>\n")
    else:
        junit_file.write(f"<failure message=\"timeout\" type=\"\"/>\n")
    junit_file.write(f"</testcase>\n")

def tc_segv(tcname, proc, elapsed):
    global fail_count
    global segv_count
    fail_count += 1
    segv_count += 1
    pcmpl = calc_percentage_complete()
    if use_colorama:
        say(f"  {pcmpl}  {Fore.BLACK}[SEGV]{Style.RESET_ALL}  {tcname} ({elapsed}s)")
    else:
        say(f"  {pcmpl}  [SEGV]  {tcname} ({elapsed}s)")
    junit_file.write(f"<testcase name=\"{tcname}\" time=\"{elapsed}\">\n")
    if proc != None:
        junit_file.write(f"<failure message=\"\" type=\"\"/>\n")
        junit_file.write(f"<system-out>\n{esc_seq(proc.stdout)}</system-out>\n")
        junit_file.write(f"<system-err>\n{esc_seq(proc.stderr)}</system-err>\n")
    else:
        junit_file.write(f"<failure message=\"SIGSEGV\" type=\"\"/>\n")
    junit_file.write(f"</testcase>\n")

def tc_pass(tcname, proc, elapsed):
    global pass_count
    pass_count += 1
    pcmpl = calc_percentage_complete()
    if use_colorama:
        say(f"  {pcmpl}  {Fore.GREEN}[PASS]{Style.RESET_ALL}  {tcname} ({elapsed}s)")
    else:
        say(f"  {pcmpl}  [PASS]  {tcname} (${elapsed}s)")
    junit_file.write(f"<testcase name=\"{tcname}\" time=\"{elapsed}\">\n")
    junit_file.write(f"<system-out>\n{esc_seq(proc.stdout)}</system-out>\n")
    junit_file.write(f"<system-err>\n{esc_seq(proc.stderr)}</system-err>\n")
    junit_file.write(f"</testcase>\n")

def tc_skip(tcname):
    global skip_count
    skip_count += 1
    pcmpl = calc_percentage_complete()
    if use_colorama:
        say(f"  {pcmpl}  {Fore.BLUE}[SKIP]{Style.RESET_ALL}  {tcname}")
    else:
        say(f"  {pcmpl}  [SKIP]  {tcname}")
    junit_file.write(f"<testcase name=\"{tcname}\" time=\"\">\n")
    junit_file.write(f"<skipped message=\"\" type=\"\"/>\n")
    junit_file.write(f"<system-out></system-out>\n")
    junit_file.write(f"<system-err></system-err>\n")
    junit_file.write(f"</testcase>\n")

def run_tc(tc):
    if tc['skip'] == True:
        tc_skip(tc['name'])
        return

    exe = cmd

    if 'cmd' in tc:
        exe = tc['cmd']

    loops = 1
    try:
        loops = tc['loop']
    except:
        pass

    global fail_count

    for i in range(loops):
        srcfile = tc['src']
        outfile = open(tc['out'], "r")
        errfile = open(tc['err'], "r")
        with open(srcfile, "r") as src:
            tcname = tc['name']
            start_time = time.time()
            try:
                p = subprocess.run([exe, srcfile],
                                   timeout=300,
                                   universal_newlines=True,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE,
                                   env=os.environ)

                tc_elapsed = time.time() - start_time
                time_str = "{:.2f}".format(tc_elapsed)
                expout = outfile.read()
                if p.stdout != expout or p.returncode != tc['exitcode']:
                    if p.returncode == -signal.SIGSEGV:
                        tc_segv(tcname, p, time_str)
                    else:
                        tc_fail(tcname, p, time_str)
                else:
                    tc_pass(tcname, p, time_str)
            except:
                tc_elapsed = time.time() - start_time
                time_str = "{:.2f}".format(tc_elapsed)
                tc_fail(tcname, None, time_str)
    outfile.close()
    errfile.close()

def validate_srcfile(tc):
    if 'src' in tc.keys():
        srcfile = tc['src']
        if not os.path.isfile(srcfile):
            raise ValueError(f"bad source path: {srcfile}")
    else:
        raise ValueError(f"missing source file")

def validate_tc(tc):
    validate_srcfile(tc)

def parse_tc(tcfile):
    global tc_repeat
    with open(tcfile, "r") as stream:
        try:
            tc = yaml.safe_load(stream)
            validate_tc(tc)
            for i in range(tc_repeat):
                run_tc(tc)
        except yaml.YAMLError as e:
            raise ValueError(f"bad yaml file: {e}")

def open_tc(tcfile):
    os.chdir(top);
    tcpath = os.path.dirname(tcfile)
    tcfile = os.path.basename(tcfile)
    tcname = os.path.splitext(tcfile)[0]
    os.chdir(tcpath)
    parse_tc(tcfile)
    os.chdir(top)

def load_tclist():
    for root, dirs, files in os.walk("test/" + test_category):
        for file in files:
            if file.endswith(".yml"):
                tclist.append(os.path.join(root, file))

    tclist.sort(key=lambda f: int(re.sub('\D', '', f)))

def exec_tclist():
    global stop_on_error
    global fail_count
    print("Running tests...")
    if use_tqdm:
        with tqdm(tclist, leave=False, unit=" test") as pbar:
            for i, tc in enumerate(tclist):
                global out
                out = pbar
                open_tc(tc)
                pbar.update(1)
                global fail_count
                if fail_count > 0:
                    break
    else:
        for tc in tclist:
            open_tc(tc)
            if fail_count > 0 and stop_on_error:
                break

def check_tc(tcfile):
    global total_tests
    with open(tcfile, "r") as stream:
        try:
            tc = yaml.safe_load(stream)
            if 'loop' in tc.keys():
                total_tests += tc['loop']
            else:
                total_tests += 1
        except yaml.YAMLError as e:
            raise ValueError(f"bad yaml file: {e}")

def scan_tclist():
    print("Scanning tests...")
    if use_tqdm:
        with tqdm(tclist, leave=False, unit=" test") as pbar:
            for i, tc in enumerate(tclist):
                global out
                out = pbar
                check_tc(tc)
                pbar.update(1)
    else:
        for tc in tclist:
            check_tc(tc)
    global total_tests
    global tc_repeat
    total_tests *= tc_repeat

def print_stats():
    global pass_count
    global fail_count
    global skip_count
    global total_tests
    if total_tests > 0:
        percentage = int((pass_count / total_tests) * 100)
    else:
        percentage = 100
    if fail_count > 0:
        print(f"\n{fail_count} tests failed, of which, {segv_count} crashed")
    else:
        print("\n0 tests failed")
    print(f"{skip_count} tests skipped")
    print(f"\n{percentage}% of {total_tests} tests passed")

start_time = time.time()

load_tclist()
scan_tclist()
exec_tclist()
print_stats()

total_time = time.time() - start_time
total_time_str = "{:.2f}".format(total_time)

print(f"Took: {str(datetime.timedelta(seconds=total_time))}")

junit_file.write("</testsuite>\n")
junit_file.close()
junit_file = open(junit_file_path, "r")
tmp_file = open("junit_tmp.xml", "w")
tmp_file.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
tmp_file.write(f"<testsuite tests=\"{total_tests}\" errors=\"0\" failures=\"{fail_count}\" skipped=\"{skip_count}\" name=\"dwt tests\" time=\"{total_time_str}\">\n")
tmp_file.write(junit_file.read())
junit_file.close()
tmp_file.close()
os.remove(junit_file_path)
os.rename("junit_tmp.xml", junit_file_path)

if fail_count > 0:
    exit(1)
else:
    exit(0)

