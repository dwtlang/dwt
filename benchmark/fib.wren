class Fib {
  static get(n) {
    if (n <= 1) return n
    return get(n - 1) + get(n - 2)
  }
}

Fib.get(40)
