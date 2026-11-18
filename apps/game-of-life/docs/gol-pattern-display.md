# Displaying Game of Life Patterns in the IER Repository

Status: presentation guide and proposed workflow\
Authority: non-canonical

Use **static generation panels** with readable captions and exact source
patterns. A composed SVG is a useful editable figure source; a PNG provides a
portable Markdown and publication fallback. CSS can arrange already computed
images on an HTML page. This guide uses no JavaScript, animated images, CSS
animation or live simulation.

The [resource guide](gol-resources.md) explains rules and interpretation. The
canonical development article `IER/IER-game-of-life.md` owns IER claims
(it is outside the current public selection);
presentation choices do not extend them.

## Choose the format for its destination

| Format | Good use | Main limitation |
| --- | --- | --- |
| Fenced `.O` text | Small seeds, diffs and exact cell inspection in Markdown | Glyph proportions vary; large boards are difficult to scan. |
| Markdown table of images | Small side-by-side phase comparisons | Narrow screens and long captions make tables awkward. |
| One composed image containing several panels | Default for an article, PDF or EPUB | Labels must remain readable at final size; provide a text description. |
| SVG rectangles and grid lines | Crisp source for a static board or multi-panel figure | Publication conversion and font behavior must be checked. |
| PNG | Broadly portable static output | Needs sufficient pixel resolution for print. |
| HTML table of cells | Tiny annotated neighborhood-count example | Verbose and accessibility-heavy for full boards; raw HTML may disappear in PDF conversion. |
| CSS Grid of cells | Controlled standalone HTML teaching page | Styling is not portable across Markdown hosts and publication engines. |
| CSS Grid of images | Responsive HTML gallery or phase strip | Needs an HTML destination that loads the stylesheet. |

The recommended repository default is a Markdown image link to a **single static
PNG contact sheet**, retaining SVG and exact cell data alongside it when useful.
Use ordinary Markdown tables for numerical observations. A board's square
geometry is better carried by an image than by hundreds of table cells.

## Make time, coordinates and the boundary visible

Every figure should state:

* Pattern name and source fixture or coordinate set.
* Rule, update schedule, board dimensions and topology, or the infinite-plane
  convention with its finite computation margin.
* Generation numbers, coordinate direction, displayed rectangle and placement
  offset. Here, coordinates increase rightward and downward.
* Whether the panels share world coordinates or are recentered independently.
* Legend and the comparison the figure supports.

Keep cell size, origin and crop fixed across motion panels. Independent tight
crops can make a glider look stationary. If a shape comparison needs recentering,
label it and separately report displacement. Show a dead-cell margin and mark
the actual board boundary separately from the image crop. The crop is a view;
it must not act as a simulation boundary.

For a radius-one rule, allow a causal margin of at least $T$ cells around the
region being assessed over $T$ updates, or compute an equivalent unbounded
evolution. A small final crop does not justify a small computation board.
For gun output, state when emitted gliders reach a finite boundary and limit
the claimed observation interval accordingly.

## A complete text example: blinker

This 5-by-5 board is bounded with dead exterior. B3/S23 updates synchronously.
The top-left site is `(0,0)`; `O` is live and `.` is dead. Each panel uses the
same world-coordinate rectangle, columns 0–4 and rows 0–4.

```text
t = 0       t = 1       t = 2
.....       .....       .....
.....       ..O..       .....
.OOO.       ..O..       .OOO.
.....       ..O..       .....
.....       .....       .....
```

Caption: The blinker returns to its initial configuration after two updates.
The central site survives each update; two sites die and two are born. Constant
population does not mean unchanged participating sites. This example is small
enough to verify directly from the rule.

For a reusable `.cells` input, retain only one board, use uppercase `O`, and
put metadata in lines starting with `!`. The current parser trims dead margins,
so placement must restore the intended world origin. The example's normalized
three-cell row needs placement at `(1,2)` to reconstruct the initial board.
See the [parser policy](cells-format.md) before treating a display block as a
loadable fixture.

## A static SVG board source

This complete SVG represents generation 0 of the same blinker. Save as a
standalone SVG if making an asset; an external Markdown image reference is
preferable to embedding raw SVG markup in an article.

```xml
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100"
     width="200" height="200" role="img" aria-labelledby="title desc">
  <title id="title">Blinker at generation zero</title>
  <desc id="desc">Five by five board. Live cells at (1,2), (2,2), (3,2).
  Coordinates start at the top left and increase rightward and downward.</desc>
  <rect width="100" height="100" fill="white"/>
  <g fill="#111">
    <rect x="20" y="40" width="20" height="20"/>
    <rect x="40" y="40" width="20" height="20"/>
    <rect x="60" y="40" width="20" height="20"/>
  </g>
  <g fill="none" stroke="#aaa" stroke-width="0.5">
    <path d="M20 0V100 M40 0V100 M60 0V100 M80 0V100
             M0 20H100 M0 40H100 M0 60H100 M0 80H100"/>
  </g>
  <rect x="0.5" y="0.5" width="99" height="99"
        fill="none" stroke="#111"/>
</svg>
```

Use a white background, dark live cells and light internal grid lines. A heavy
outer stroke here marks the actual finite board. For a crop of a larger world,
use a lighter frame and label it as a crop. SVG source needs no scripts,
external resources or interactive elements.

For multi-panel SVGs, give each panel a fixed translation and add generation
labels outside its board. Keep readable text as text or provide an equivalent
caption. An SVG's internal description may not be exposed when embedded as an
image, so the surrounding Markdown still needs useful alt text and prose.

## Optional HTML gallery with CSS

The following is a template for a controlled HTML publication. The example
filenames are proposed assets, not files currently supplied by the simulator.
Generate the images first. The stylesheet only arranges them.

```html
<div class="gol-panels" aria-label="Blinker phase comparison">
  <figure>
    <img src="blinker-g000.png" width="400" height="400"
         alt="Generation 0: live cells (1,2), (2,2), (3,2).">
    <figcaption>t = 0 — horizontal phase</figcaption>
  </figure>
  <figure>
    <img src="blinker-g001.png" width="400" height="400"
         alt="Generation 1: live cells (2,1), (2,2), (2,3).">
    <figcaption>t = 1 — vertical phase</figcaption>
  </figure>
</div>
<p>B3/S23, synchronous; 5 × 5 bounded board, dead exterior.
Both panels show the same coordinates; the initial phase returns at t = 2.</p>
```

```css
.gol-panels {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(min(100%, 12rem), 1fr));
  gap: 1rem;
}
.gol-panels figure { margin: 0; break-inside: avoid; }
.gol-panels img { display: block; max-width: 100%; height: auto; }
.gol-panels figcaption { margin-top: .4rem; }
@media print {
  .gol-panels { display: block; }
  .gol-panels figure { margin-bottom: 1rem; }
}
```

Use a linear reading order and visible captions. Do not depend on color or
hover to convey births, deaths or phase differences. For a larger board, alt
text should explain the pattern and comparison; put exact coordinates in an
adjacent linked text file rather than an enormous alt attribute.

If using an HTML table for a neighborhood demonstration, include a caption,
row/column headers and textual state labels. Empty colored cells alone do not
make an accessible data table. CSS Grid of individual cells is reasonable for
one small controlled example, but a compact image plus exact text is easier
to reuse across this repo's publication routes.

## Where files belong

Use the existing separation of responsibilities:

* `docs/`: general pattern explanations and display instructions. A proposed
  `docs/figures/` can hold the corresponding static assets.
* `research/worked-examples/`: IER-specific comparisons and interpretation;
  its existing `fixtures/` contains reproducible starting boards. Keep research
  figure assets next to the worked example that explains them.
* `IER/IER-game-of-life.md`: canonical explanation. Adding a figure here is a
  separately reviewed article edit; a new guide does not install it.
* `data/`: possible home for shared publication figures used by multiple
  articles. Follow the owning publication's existing conventions.
* `_work/` and `build/`: working reports and generated outputs. Avoid making
  durable article links depend on disposable files there.

Prefer names such as `glider-b3s23-g000-004.svg` and
`glider-b3s23-g000-004.png`. Keep the fixture or exact coordinate data, rule,
crop, generation list and generator instructions in a neighboring Markdown
note. For third-party patterns, record provenance and any reuse conditions;
create fresh diagrams from verified cell data instead of copying screenshots.

Markdown image paths should be relative to the document containing them. For
example, a future figure placed in `docs/figures/` could be referenced from a
doc as:

```markdown
![Glider phases 0–4 on a fixed crop; final phase shifts one cell right and down.](figures/glider-b3s23-g000-004.png)
```

This is a naming example, not an installed asset. Captions should distinguish
an exact computed trace from a schematic drawing or reconstructed example.

## Publication integration and reproducible generation

The development repo uses Pandoc and XeLaTeX; inspect `mk/pandoc.mk`
there before wiring in figures (build tooling is outside the public app
projection). Its shared resource path currently includes `.` and
`IER`, but does not include this app's `docs/` or `research/` directories.
Source-relative image links in assembled content may therefore need the owning
build's resource path extended or explicit asset staging. Do not assume a link
that renders locally will resolve after several Markdown files are assembled.

Raw HTML and CSS layouts are destination-specific. For PDF and EPUB, prefer
ordinary Markdown image syntax and a precomposed figure. Use PNG as the simple
fallback; an SVG may require a conversion dependency in the PDF route. Supply
a PDF derivative only where the owning build supports it. Verify the actual
publication output before claiming format support.

The app currently has no headless snapshot/export command. Its ANSI terminal
display is a view of runtime state, and terminal resize can change the world.
A terminal screenshot therefore needs more provenance than a decorative image
and is usually a poor source for a clean cell diagram. The existing
[world checks](../tests/README.md) and
[worked coordinate traces](../research/worked-examples/glider.md) provide
verification references, rather than a figure-export pipeline.

For a future offline exporter, add an owning Make target rather than a manual
sequence that becomes undocumented build logic. It should:

1. Read an exact fixture and explicit placement, dimensions, topology and rule.
2. Compute selected generations through the existing world implementation, or
   verify independently computed boards against it.
3. Render immutable snapshots without feeding display crops back into updates.
4. Emit SVG/PNG and a provenance record with crop, generations and source
   revision. Never infer generation numbers from elapsed wall time.
5. Produce the same figures from the same declared inputs.

No exporter or new build target is implemented by this guide. Small examples
can be drawn directly from checked coordinates, with that origin declared.

Before accepting a figure, compare its live sites to the source, check the
claimed phase relation and topology, inspect it at final print size and in
grayscale, and verify image resolution and links in each intended destination.
For a glider, compare generations 0 and 4 in fixed coordinates. For a toad,
show both phases. For a gun, show the core and emitted output at labeled
checkpoints; a single phase picture cannot demonstrate emission.
