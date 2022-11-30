# this is not a full script, use it with source

# file locator
SOURCE="${BASH_SOURCE[0]:-$0}";
while [ -L "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
        DIR="$( cd -P "$( dirname -- "$SOURCE"; )" &> /dev/null && pwd 2> /dev/null; )";
        SOURCE="$( readlink -- "$SOURCE"; )";
        [[ $SOURCE != /* ]] && SOURCE="${DIR}/${SOURCE}"; # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRIPT_DIR="$( cd -P "$( dirname -- "$SOURCE"; )" &> /dev/null && pwd 2> /dev/null; )";



cd "$SCRIPT_DIR"

TS=`date '+%F-%s'`
OUTPUT_DIR="$SCRIPT_DIR/../results/$TS$SUFFIX/"

mkdir -p "/tmp/enablement"
ENABLE_FILE="/tmp/enablement/$ENABLER_NAME"
echo 0 > "$ENABLE_FILE"

enable_trace()
{
	echo 1 > "$ENABLE_FILE"
}
