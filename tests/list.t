# vim: set ts=2 sw=2 expandtab:

Command `list` without any plugin registered show a message.

  $ $ZPM reset
  $ $ZPM list
  Nothing to show.
  [1]

Can create a new list.

  $ $ZPM reset
  $ $ZPM "$ZPM_TEST_REPOS/example-plugin"
  Installing .*/example-plugin... Done. (re)

  $ cat $HOME/.zpm-init.zsh
  source .*/example-plugin/example.plugin.zsh (re)
  fpath\+=.*/example-plugin (re)
  PATH=.*/example-plugin:\$PATH (re)
  autoload -Uz compinit; compinit -iCd \$HOME/.zcompdump (re)

Subsequentially add a new item to the list.

  $ $ZPM "$ZPM_TEST_REPOS/example-theme"
  Installing .*/example-theme... Done. (re)
  $ cat $HOME/.zpm-init.zsh
  source .*/example-plugin/example.plugin.zsh (re)
  fpath\+=.*/example-plugin (re)
  PATH=.*/example-plugin:\$PATH (re)
  source .*/example-theme/example.zsh-theme (re)
  fpath\+=.*/example-theme (re)
  PATH=.*/example-theme:\$PATH (re)
  autoload -Uz compinit; compinit -iCd \$HOME/.zcompdump (re)

List command show list of installed plugins.

  $ $ZPM list
  .*example-plugin@.* (re)
  .*example-theme@.* (re)

Do not allow duplicate plugin entries/items.

  $ $ZPM reset
  $ $ZPM "$ZPM_TEST_REPOS/example-theme" > /dev/null
  $ $ZPM "$ZPM_TEST_REPOS/example-theme" > /dev/null
  [1]
  $ $ZPM list
  .*example-theme@.* (re)
