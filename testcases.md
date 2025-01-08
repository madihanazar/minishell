Test cases to check echo:

echo hello
echo
echo ""
echo ''

echo "hello"
echo 'hello'
echo "hello world"
echo 'hello world'
echo "hello   world"    # multiple spaces inside quotes
echo "'hello'"         # single quotes inside double quotes
echo '"hello"'         # double quotes inside single quotes

echo hello      world    # multiple spaces between words
echo    hello world      # spaces before first word
echo hello world        # spaces after last word

echo -n hello
echo -n            # just -n option
echo -n -n hello   # multiple -n options
echo hello -n      # -n as regular argument
echo -nnnnn hello  # multiple n's

export TEST="hello world"
echo $TEST
echo "$TEST"
echo '$TEST'
echo "hello $TEST"
echo 'hello $TEST'

echo hello"world"
echo "hello"world
echo "hello"'world'
echo "$TEST"world
echo $TEST"world"

echo \n           # backslash
echo "\"hello\""  # escaped quotes
echo "\\"         # escaped backslash
echo "$"          # single dollar sign
echo "$ "         # dollar sign with space
