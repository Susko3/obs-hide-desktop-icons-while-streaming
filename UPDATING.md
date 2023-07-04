# How to update the base template to the latest `master`

Ensure a clean working tree

```shell
git reset --hard
```

Pull the latest template data.

```shell
git remote add template https://github.com/obsproject/obs-plugintemplate.git
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