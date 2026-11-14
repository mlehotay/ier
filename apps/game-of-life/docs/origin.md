# Origin of the Game of Life

Status: historical reference\
Authority: non-canonical

John Horton Conway developed the Game of Life at Cambridge, experimenting with
simple rules that could produce prolonged, varied population behavior. Martin
Gardner introduced it to a wide audience in his October 1970 *Scientific
American* column; the original presentation used counters on a checkerboard.

Conway called it “Life.” The name reflects its biological imagery: cells are
born, survive or die according to their neighbors, and populations can grow,
disappear, stabilize or oscillate. Gardner explicitly connected the game with
changing societies of organisms. This explains the naming context without
implying that the cells are literally living organisms.

Life is a zero-player mathematical game: after an initial configuration and
the rules are fixed, its generations require no further player decisions.
The conventional rule is now written B3/S23. All sites update simultaneously,
so a birth in one generation cannot influence another site's calculation for
that same generation. The birth, survival and death vocabulary describes
binary state changes on a lattice.

Gardner's column is also a record of an early research moment. Its challenge
about unlimited population growth should be read in that historical context.
The repo's [Gosper gun checkpoints](patterns.md#gun-emission-checkpoints)
illustrate repeated emission on a finite board over a specified interval;
unbounded-growth statements concern the ideal unbounded model.

For the wider rule family and philosophical uses, see
[Game of Life resources](gol-resources.md). For diagrams that work without
live simulation, see the [static pattern display guide](gol-pattern-display.md).

Source: Martin Gardner, [“The fantastic combinations of John Conway’s new
solitaire game ‘life’”](https://www.ibiblio.org/lifepatterns/october1970.html),
*Scientific American* 223 (October 1970), pp. 120–123.
