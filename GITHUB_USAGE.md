# GitHub Usage/Instructions

Alright, here is the GitHub details. Not urgent as we are working on the design review right now, but I figured I'd get ahead while I have some time.

**Overview**
I took the latest FULL (xfig/fig2dev) Xfig install to the following new GitHub repo. I also added a simple readme, which I will clean up in the future. This readme also has some install instructions to build xfig itself (no idea for fig2dev yet). I also added a list of the added branches.

**Branches**
I added 5 branches in addition to the main branch. Four that each of you can have total control over for development purposes for each feature. Once we have things working, we can move feature changes to the integration branch, where we can as a group approve a change and ensure it integrates with the rest of the src code well. This branch can then be published to main when we are happy with it. Main should only be updated for major releases. At the moment these branches are identical, but we can start using them whenever.

| Branch Name                 | Description                                                                              |
|-----------------------------|------------------------------------------------------------------------------------------|
| **main**                    | The main release branch.                                                                 |
| **integration**             | A staging branch to integrate changes together for release to main.                      |
| **export-2-vdx-dev**        | A branch for development of the Export to Visio feature.                                 |
| **layer-ctrl-dev**          | A branch for the development of the Layer Control (Bring to Back/Send to Front) feature. |
| **undo-redo-dev**           | A branch for the development of the better Undo/Redo functionality feature.              |
| **quick-menu-dev**          | A branch for the development of the Quick Menu feature.                                  |

For more information on branches, see: https://git-scm.com/book/en/v2/Git-Branching-Branches-in-a-Nutshell

**GitIgnore**
I added a gitignore file for generated build files such as .o. The only thing we should need to commit is src .c/.h files.

I also added a script ```git-ignore-tracked.sh``` for ignoring files that are tracked (in GitHub) but change with the build such as the temp makefile. Some of these may need to be removed later, but I did it to declutter the ```git status``` menu. This script will need to be run after compiling XFig for the first time. I can explain more on the ignore files later if you guys want, but for now, as a general rule, **Only commit *.c/*.h files or changes to the makefiles**

**Installation Notes**
All steps should be in the readme page on GitHub. Let me know if you have further questions. I want to make sure we are all comfortable with this.

**Access and Configuration**
In order to push/pull to the GitHub, you all need to be added to the repo. **DM me your GitHub ID if I don't have it already so I can add you guys.**

I'd recommend pulling the http link as shown in the readme unless you want to use sshkeys and feel comfortable with this. I can help with this if needed.

Before committing, you will need to add your Git email and name as follows:

```bash
$ git config --global user.name "<YOUR NAME>"
$ git config --global user.email "<YOUR GITHUB EMAIL>"
```

For example, for me (note personal dorky 13 yr old GitHub email):

```bash
$ git config --global user.name "Chris Lloyd"
$ git config --global user.email "Legoman3267@gmail.com"
```

You can also optionally save your credentials (username and pass for GitHub) for pushing and pulling to GitHub using Git's credential manager.

See https://git-scm.com/book/en/v2/Git-Tools-Credential-Storage for more information on this. For simplicity, I'd suggest saving your credentials by doing the following:

```bash
git config --global credential.helper store # Tell Git to save your credentials next time
git pull                                    # Enter your credentials for your GitHub account (these will be saved)
git pull                                    # This time it should not prompt you
```

***General Git/GitHub Workflow***
This is a quick crash course into these two tools and some common commands.

First of all, just so we are all on the same page, **GitHub** is the "server" that is hosting our **Git** repo. Git allows us to all make changes to the code, commit them locally, then push our copy of the git repo with these commits to the main "server" repo on GitHub.

Once the repo is all setup, git will default to the "main" or master branch. This can be verified with a ```git status``` No commits should go to here. Instead, another branch should be checked out for a specific feature as follows:

```bash
git checkout <BRANCH NAME>
```

Branch names can be listed using the following commands:

```bash
git branch -r      # List origin (on GitHub) branches
git branch         # List local branches (branches that have been checked out before on a specific git repo)
```

So for example, to change to the Export to Visio branch for Zacch to make progress:

```bash
git checkout export-2-vdx-dev
```

This will create a new local branch to point to the origin's export-2-vdx-dev branch. If you want to jump back to the main branch with local changes (both uncommitted and committed), do the following. Note this will save all changes in the export branch. This could be useful if you are making changes and want to see how the clean default install looks, you can swap to main, build it/run XFig then hop back to the feature branch.

```bash
git checkout main
# Do something
git checkout export-2-vdx-dev
```

When in any part of the repo on any branch, the following command is the goto command to see the state of the repo, what changes there are, what's added, how many commits ahead/behind your feature is, or what branch you are in. This is like the most used command.

``` bash
git status
```

When you have made some changes and want to save them, the following general workflow should be used:

```bash
git status                         # Check the state of the repo (what files are staged/added and ready for commit or what files are ready to be staged/added)
git add <PATH TO CHANGED FILES>    # Add (stage) the unstaged files (can use wildcards like * "Carefully")
git status                         # Confirm that only the files you want are staged
git commit                         # Commit the staged changes (This will open a text editor (nano) and prompt you for a commit message (more on these later))
git status                         # Confirm the local change
```

As a habit, commit as often as possible (rather try to split tasks into tiny logical steps so you can save as you go). There is a balance to this as too many commits can clutter, however, having one per coding session is to few in my eyes. Try to make commits like legos, choose tiny logical blocks that may make sense to add/remove later for tweaks/functionality.

For now don't worry about it, but if you want to delete or modify/split commits, msg me and we can go through it. It's a scary process as you could screw up other commits, but in the end it's not that hard. Eventually, all of you will need this, but it's easier to explain/demo then write text about.

As for commit messages, make them as detailed as possible. Like variable naming, they should represent the change that was made. Use the style "Added/Modified xyz" rather than "Add/modify xyz". See https://chris.beams.io/posts/git-commit/ for more details. Having good commit msgs will be rewarding later.

Lastly, as for when to push, I only want code pushed on GitHub if it is working in some fashion. That is to say it compiles and "runs". It may not be a full feature, but if it kind of works, it's okay in my eyes. I would rather tons of commits on your end, then we rebase/clean them up (talk to me later) then push them so that GitHub does not get that cluttered. However once again, for all practical purposes, feature champions "own" your feature branch so you get to choose this style.

```bash
git push
git pull
```


Git is a complicated thing and has lots of confusing commands. it's pretty Googlable, but you can still screw yourselves up pretty bad. If something is not working, or Git is giving error messages, or you want to do something but don't know how, DM me. The worst thing you can do is do a ```git push``` in these cases. That's how we lose information. If you contact me we can work it out and keep things organized and running smoothly.
