web_dir="/var/www/html"

copy_files()
{
    if [[ $1 == "" ]]; then return; fi
    for file in *.$1
    do
        echo "copying $file to $web_dir"
        cp $file $web_dir
        if [[ $? -ne 0 ]]; then exit 1; fi
    done
}

rm $web_dir/* -rf
copy_files "html"
copy_files "css"
copy_files "js"
copy_files "php"
copy_files "png"
copy_files "svg"