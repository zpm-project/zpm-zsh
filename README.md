# zpm - Zsh Plugin Manager [![Gitter](https://badges.gitter.im/Join Chat.svg)](https://gitter.im/zpm-project/Lobby) [![Build Status](https://travis-ci.org/zpm-project/zpm-zsh.svg)](https://travis-ci.org/zpm-project/zpm-zsh)

zsh plugin manager in ansi C.

### Installation

Clone this repository and then:

```sh
make

```

You'll have a `zpm` binary in the current directory, copy it to `/usr/local/bin`.

### Usage

#### Installing plugins

You first will need to install the plugins you want:

```sh
zpm "zsh-users/zsh-syntax-highlighting"
zpm "zsh-users/zsh-autosuggestions"
zpm "rupa/z"
```

These commands will generate `$HOME/.zpm-init.zsh` file. Use that to load plugins.

#### Configure zsh

Then configure zsh to use them:

```sh
# .zshrc
source $HOME/.zpm-init.zsh
```

If you add or remove new plugins you will need to:

```sh
zpm reset
```

To see a list of installed plugins:

```sh
zpm list

```
To update installed plugins:

```sh
zpm update

```

To remove plugin:

```sh
zpm remove "user/plugin"
zpm remove "/path/to/local/plugin"
```

To uninstall plugin:

```sh
zpm uninstall "user/plugin"
zpm uninstall /path/to/local/plugin
```

To make a copy of your config

```sh
zpm save > .zpmrc
```

## Development

Running tests:

```sh
make test
```

You'll need zcram (or cram) installed and available in `PATH`.

## Maintainers

`zpm` is currently being actively developed by @desyncr and @fennecdjay.

## TODO

  - [X] Update command
  - [X] Add plugins to fpath
  - [X] Support for local plugins
  - [X] List command show version (hash)
  - [X] Add executables to PATH
  

