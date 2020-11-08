echo "`n`nListing files marked NOT to be pushed/synced by git`n`n"


bash -c 'git ls-files -v|grep "^h"'