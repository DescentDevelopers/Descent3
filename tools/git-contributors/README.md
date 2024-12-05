# git-contributors.py

**git-contributors.py** - script to generate contributors list from git history.

## Requirements

* Recent python (3.12+)
* GitPython package (https://pypi.org/project/GitPython/)

## Usage

Just run `git-contributors.py`, review and commit `scripts/data/fullhog/oscredits.txt`.
You're now awesome.

**Remember regenerate file before releasing new version!**

## Customization

Tool provides two ways of customization:

* Exclude particular name from list of contributors (file `no-add.txt`,
  one name per line, comments begins with `#`).
* Replace name in list of contributors with other one (file `map-names.txt`,
  one entry per line in format `git_name:replace_name`, comments begins with `#`).
