# USBNugget software development process

## 1. Work planning and task creation

For now, we will decide what work needs to be done during our weekly meeting.
Work that needs doing is listed on [this project's Kanban
board](https://github.com/orgs/HakCat-Tech/projects/4) under the "To do"
column. Work that needs doing but is not fully defined can be added to the
"Backlog" column.

## 2. Communicate intended implementation

Extremely trivial changes require no communication, other than updating the
Kanban board to reflect what you'll be working on.

When implementing non-trivial changes, share your ideas before implementing
them.
- For minor features, a short discussion during regular meeting or over chat is
  probably sufficient
- For major features, strongly consider creating a design document. This allows
  other developers to comment on the design, suggest improvements or
  alternatives, and think about edge cases. This prevents wasting time
  constructing flawed implementations, and provides future developers with
  artifacts to understand the code base and past decisions. Design documents
  should be as short as possible while fully capturing your ideas. Use Google
  docs or some other service that allows others to easily add comments in
  specific sentences/paragraphs.

## 3. Implementation

### Style guide
Overly strict adherence to a style guide is unproductive. If there is
disagreement between two valid methods of implementation, a style guide can
serve as an unopinionated third-party to resolve the conflict. Code should
loosely follow the guide, and reviewers can consult the guide to suggest code
improvements.

For this project, we will be using the [Google C++ style
guide](https://google.github.io/styleguide/cppguide.html). If we find that it
doesn't meet our needs, we can copy and modify it or choose another entirely.

### Use git commit best practices
See [this
gist](https://gist.github.com/luismts/495d982e8c5b1a0ced4a57cf3d93cf60) for
some best practices. In general, things can be as messy as you want during
development, but clean up before submitting a PR. Break large commits into
smaller ones when it's sensible.

### Documentation
Implementing larger features may require updates to documentation.
Documentation should be updated alongside implementation of these features in
the same commit or PR.
- Product-specific documentation in the README file in the project directory root.
- Developer-specific documentation belongs in `/src/docs`. It should be written in markdown.

### Write comments as needed
See [best practices for writing code comments](https://stackoverflow.blog/2021/12/23/best-practices-for-writing-code-comments/)

### Keep the Kanban board updated
Keeping [the Kanban board](https://github.com/orgs/HakCat-Tech/projects/4)
updated ensures that it remains the source of truth for the current state of
planned, in-progress, and completed code changes.

- When first starting work on a feature, bugfix, or anything else that will
  result in a PR, please create (or find) a ticket in the GitHub issue tracker
  and assign it to yourself. Add it to the project's Kanban under the "in
  progress" column. This prevents developers from accidentally doing the
  duplicate work.
- When you submit your PR for review, move the note/issue into the "Review in
  progress column".
- When your PR is merged, move the note/issue to the "Done" column.
- If the task you're working on becomes blocked, add the "blocked" label.


## 4. Code review
Every change to the code base should have at least one reviewer.

### What a reviewer should look for
- Descriptive and accurate variable/function names
- Corresponding documentation changes are in the PR 
- Code that could easily cause memory leaks
- Dead code/cruft: this includes blocks of code that are commented out,
  functions that aren't called, 
- Adherence to common patterns in the code base
- Comments should be useful and not duplicate the code. When code changes
  around an existing comment, the comment should be checked to make sure it is
  still accurate. See [best practices for writing code
  comments](https://stackoverflow.blog/2021/12/23/best-practices-for-writing-code-comments/)

### Submitting code
On github, there are a few options available to you when submitting your code.
For most changes, the "squash and merge" option is best. This combines commits
from your PR into a single commit and merges it. This makes it easier to
understand git history.

### Allow the PR author to submit the pull request
There may be a reason the PR author hasn't merged, and generally it's
considered good etiquette.

[This answer](https://softwareengineering.stackexchange.com/a/334496) on
stackoverfow words it nicely:

> Sometimes the author becomes aware of reasons later that a pull request
> shouldn't be merged yet. Maybe another developer's PR is higher priority and
> would cause conflicts. Maybe she thought of an uncovered use case. Maybe a
> review comment triggered a brainstorm that needs further investigation before
> the issue is fully satisfied. The author knows the most about the code, and it
> makes sense to give him or her the last word about when it gets merged.

