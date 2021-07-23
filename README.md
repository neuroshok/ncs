# Neuroshok Command System

## How to use

`ncs project add`
Missing parameter <name>

`ncs project add -name` not working because name has no default

`ncs project add -name:my_project`
Ok

`ncs project add -name:my_project -vcs:true`

`ncs project add -name:my_project -vcs`
Use vcs default value


name, "zeta", required
name, required

if (no name && required) use default
if (-name && required) use default

vcs: optional, default: git