"""Small regression suite for generated Processor assembly.

The Processor repository currently lacks Comp.h, so this checks the emitted
instruction subset with a tiny interpreter instead of its binary.
"""

from pathlib import Path
import subprocess
import tempfile


COMPILER = Path(__file__).resolve().parents[1] / "read.exe"


def run_assembly(filename):
    instructions = []
    labels = {}
    for raw in filename.read_text().splitlines():
        line = raw.strip()
        if not line:
            continue
        if line.endswith(":"):
            labels[line[:-1]] = len(instructions)
        else:
            instructions.append(line.split())

    ram = [0] * 10000
    stack = []
    returns = []
    output = []
    ax = 0
    ip = 0
    steps = 0

    def read(argument):
        if argument == "ax":
            return ax
        if argument.startswith("["):
            return ram[int(argument[1:-1])]
        return int(argument)

    while ip < len(instructions):
        steps += 1
        assert steps < 100000, "Execution limit exceeded"
        command, *args = instructions[ip]
        ip += 1

        if command == "push":
            stack.append(read(args[0]))
        elif command == "pop":
            value = stack.pop()
            if args[0] == "ax":
                ax = value
            else:
                ram[int(args[0][1:-1])] = value
        elif command in ("add", "sub", "mul", "div"):
            right = stack.pop()
            left = stack.pop()
            if command == "add":
                stack.append(left + right)
            elif command == "sub":
                stack.append(left - right)
            elif command == "mul":
                stack.append(left * right)
            else:
                stack.append(int(left / right))
        elif command == "out":
            output.append(stack.pop())
        elif command == "call":
            returns.append(ip)
            ip = labels[args[0]]
        elif command == "ret":
            ip = returns.pop()
        elif command == "jmp":
            ip = labels[args[0]]
        elif command in ("ja", "jae", "jb", "jbe", "je", "jne"):
            right = stack.pop()
            left = stack.pop()
            compare = {
                "ja": left > right,
                "jae": left >= right,
                "jb": left < right,
                "jbe": left <= right,
                "je": left == right,
                "jne": left != right,
            }
            if compare[command]:
                ip = labels[args[0]]
        elif command == "hlt":
            break
        else:
            raise AssertionError(f"Unknown instruction: {command}")

    return output


PROGRAMS = {
    "factorial": ("""
/:first_half
    start_game SSUUII 1 whistle
    end_game pass stadium_open n pressing 1 stadium_close
    SSUUII n assist transfer $factorial$ stadium_open offside n foul 1 stadium_close whistle
/:second_half GOAT $factorial$ stadium_open GOAT n stadium_close
/:first_half
    VAR transfer $factorial$ stadium_open offside 5 stadium_close whistle
/:second_half GOAT $main$ stadium_open stadium_close
""", [120]),
    "fibonacci": ("""
/:first_half
    start_game SSUUII n whistle
    end_game pass stadium_open n pressing 1 stadium_close
    SSUUII transfer $fib$ stadium_open offside n foul 1 stadium_close goal
           transfer $fib$ stadium_open offside n foul 2 stadium_close whistle
/:second_half GOAT $fib$ stadium_open GOAT n stadium_close
/:first_half
    VAR transfer $fib$ stadium_open offside 8 stadium_close whistle
/:second_half GOAT $main$ stadium_open stadium_close
""", [21]),
    "blocks": ("""
GOAT x score 10 whistle
/:first_half
    GOAT x score 20 whistle
    start_game GOAT x score 30 whistle
        VAR x whistle
    end_game pass stadium_open x leader 0 stadium_close
    VAR x whistle
    start_game GOAT x score 40 whistle
        x score x foul 1 whistle
    end_game dribble stadium_open x underdog 0 stadium_close
    VAR x whistle
/:second_half GOAT $main$ stadium_open stadium_close
""", [30, 20, 20]),
    "side_effect": ("""
/:first_half
    VAR 7 whistle
    SSUUII 2 whistle
/:second_half GOAT $effect$ stadium_open stadium_close
/:first_half
    VAR 0 assist transfer $effect$ stadium_open stadium_close whistle
/:second_half GOAT $main$ stadium_open stadium_close
""", [7, 0]),
    "two_parameters": ("""
/:first_half
    SSUUII left foul right whistle
/:second_half GOAT $subtract$ stadium_open GOAT left GOAT right stadium_close
/:first_half
    VAR transfer $subtract$ stadium_open offside 9 offside 4 stadium_close whistle
/:second_half GOAT $main$ stadium_open stadium_close
""", [5]),
    "mutual_recursion": ("""
/:first_half
    start_game SSUUII 1 whistle
    end_game pass stadium_open n draw 0 stadium_close
    SSUUII transfer $odd$ stadium_open offside n foul 1 stadium_close whistle
/:second_half GOAT $even$ stadium_open GOAT n stadium_close
/:first_half
    start_game SSUUII 0 whistle
    end_game pass stadium_open n draw 0 stadium_close
    SSUUII transfer $even$ stadium_open offside n foul 1 stadium_close whistle
/:second_half GOAT $odd$ stadium_open GOAT n stadium_close
/:first_half
    VAR transfer $even$ stadium_open offside 6 stadium_close whistle
/:second_half GOAT $main$ stadium_open stadium_close
""", [1]),
}


def main():
    assert COMPILER.exists(), "Run make before tests"
    with tempfile.TemporaryDirectory() as directory:
        location = Path(directory)
        source = location / "input.txt"

        for name, (program, expected) in PROGRAMS.items():
            source.write_text(program)
            result = subprocess.run([COMPILER, source], cwd=location,
                                    capture_output=True, text=True)
            assert result.returncode == 0, f"{name}: {result.stderr}"
            actual = run_assembly(location / "output.asm")
            assert actual == expected, f"{name}: {actual} != {expected}"
            print(f"{name}: OK")

        source.write_text("""
/:first_half
    start_game GOAT hidden score 1 whistle
    end_game pass stadium_open 1 draw 1 stadium_close
    VAR hidden whistle
/:second_half GOAT $main$ stadium_open stadium_close
""")
        result = subprocess.run([COMPILER, source], cwd=location,
                                capture_output=True, text=True)
        assert result.returncode != 0 and "Unknown variable 'hidden'" in result.stderr
        print("out_of_scope: OK")

        source.write_text("""
/:first_half SSUUII n whistle
/:second_half GOAT $identity$ stadium_open GOAT n stadium_close
/:first_half
    VAR transfer $identity$ stadium_open stadium_close whistle
/:second_half GOAT $main$ stadium_open stadium_close
""")
        result = subprocess.run([COMPILER, source], cwd=location,
                                capture_output=True, text=True)
        assert result.returncode != 0 and "needs 1 arguments, got 0" in result.stderr
        print("wrong_arity: OK")

        source.write_text("""
/:first_half
    VAR 1
/:second_half GOAT $main$ stadium_open stadium_close
""")
        result = subprocess.run([COMPILER, source], cwd=location,
                                capture_output=True, text=True)
        assert result.returncode != 0 and "missing 'whistle' before '/:second_half'" in result.stderr
        assert "Assertion" not in result.stderr
        print("missing_whistle: OK")


if __name__ == "__main__":
    main()
