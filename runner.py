import subprocess
import sys
import scipy
from itertools import combinations


def run_executable(func):
    cmd = ["./build/integrate_serial", f"func{func}.cfg", str(func)]
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout = result.stdout.decode("utf-8")
    exit_code = result.returncode
    if exit_code != 0:
        print(f"Error in {func} func:", result.stderr.decode("utf-8"))
        exit(exit_code)
    else:
        return stdout


def analyze(repetitions):
    results = []
    for func in range(1, 4):
        integrals, times = [0] * repetitions, [0] * repetitions
        for i in range(repetitions):
            integrals[i], abs_err, rel_err, times[i] = list(map(float, run_executable(func).strip().split("\n")))

        if all(abs(i - j) < 10 ** (-7) for i, j in combinations(integrals, 2)):
            results += [f"{integrals[0]}\n{abs_err}\n{rel_err}\n{int(min(times))}\n" +
                        f"{int(sum(times) / repetitions)}\n{int(scipy.stats.tstd(times)) if repetitions > 1 else 0}"]
        else:
            results += ["Different values of integers.", "\n"]
    print("\n\n".join(results))


def parser():
    if len(sys.argv) == 2:
        if int(sys.argv[1]) < 1:
            print("Incorrect number of repetitions.")
            exit(1)
        return int(sys.argv[1])
    else:
        print("Incorrect amount of argument.")
        exit(1)


def main():
    analyze(parser())


if __name__ == "__main__":
    main()
