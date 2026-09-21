## concepts

- command: first-level command. e.g., **git** commit -m "Fix bugs"
- subcommand: second or higher level command. e.g., git **commit** -m "Fix bugs"
- argument: parameter of a (sub)command. e.g., git commit **-m** **"Fix bugs"** These two are separated arguments
- register: bind a command handling function to a specific command
- dispatch: call a command handling function with arguments ahead
- branch: various ways of a command
