input="$1";
output="$2";
function fileExists {
	if ! [ -f "$1" ]; then
  		echo "File $1 does not exist."
		exit 1
	fi
}
if [ -z "$input" ] || [ -z "$output" ]; then
    lines=()
    while IFS= read -r line; do
        lines+=("$line")
    done
    for index in "${!lines[@]}"; do
        echo "$((index + 1)). ${lines[$index]}"
    done
    exit 1
fi


fileExists "$input"
fileExists "$output"

awk 'BEGIN{i=1} /.*/{printf "%d. % s\n",i,$0; i++}' $input > $output 
