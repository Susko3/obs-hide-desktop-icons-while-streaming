# How to update the base template to the latest `master`

## Setup

```shell
git remote add template https://github.com/obsproject/obs-plugintemplate.git
```

## Updating

Ensure a clean working tree

```shell
git reset --hard
```

Pull the latest template data.

```shell
git fetch --all -p
git checkout template/master --pathspec-from-file pathspec
```

Check which files have changed,

```shell
git status --verbose
```

restore files that should stay,

```shell
git restore --worktree --staged src
git restore -WS buildspec.json
git restore -WS CMakeLists.txt
```

and then commit.

```shell
git commit
```

Remeber to write down the commit hash that you're updating to!
```shell
git log -1 template/master
```
