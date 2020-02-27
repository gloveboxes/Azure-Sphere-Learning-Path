echo "`n`nSetting --no-assume-unchanged on all app_manifest.json files."
echo "app_manifest.json files will be pushed/synced by git`n`n"

$fileList = Get-ChildItem -Include app_manifest.json -Recurse  -Name -Depth 2
foreach ($file in $fileList) 
{
    echo $file
    git update-index --no-assume-unchanged $file
}