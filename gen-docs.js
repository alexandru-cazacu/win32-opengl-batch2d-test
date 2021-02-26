// @ts-check

/**
 * GenDocs default Config.
 */
const config = {
  srcDir: "./src",
  title: "Hyper | API Docs",
  lang: "en",
  projectName: 'Hyper',
  issuesUrl: 'www.com'
};

const path = require("path");
const fs = require("fs").promises;

const startHtml = `<meta charset="utf-8" lang="${config.lang}"><style class="fallback">body{visibility:hidden;}</style>`;
const endHtml = `<!-- Markdeep: --><head><title>${config.title}</title></head><link rel="stylesheet" href="https://casual-effects.com/markdeep/latest/apidoc.css"><script src="https://morgan3d.github.io/markdeep/latest/markdeep.min.js"></script>`;

const testSrc = `
/// Example function.
static void foo();

/// Example struct.
typedef struct
{

} Fool;

/// Example variable.
int Bar;

/// Example deprecated variable.
/// @deprecated
int DeprecatedBar;

`;

(async () => {
  const files = [];
  for await (const filePath of getFiles(config.srcDir)) {
    const fileData = (await fs.readFile(filePath, { encoding: "utf8" })).split("\n");
    files.push({
      /** @type {string} */
      path: filePath,
      /** @type {string[]} */
      data: fileData,
    });
    // const res = parseSource(fileData);
  }

  const htmlTemplate = `
${startHtml}

![](./media/banner.png)

# Hyped <span style="color:lightgray"> - Editor of the future, today.</span>

Brief project description.

## Files

${files.map((filePath) => "- **" + filePath.path.replace('C:\\dev\\hyped\\src\\', '') + '** - Pippo').join("\n")}

## FooModule <span style="color:lightgray">#include "foo_file.c"</span>

FooModule brief description.

### internal void FooNew(); ![](https://img.shields.io/badge/-new_v1.0-green?style=plastic)

Brief description of this function.

More exhaustive description.

### internal void FooDeprecated(); ![](https://img.shields.io/badge/-deprecated-red?style=plastic)

Brief description of this function.

More exhaustive description.

!!! ERROR: Deprecated
    This function is deprecated, use ... instead.

## FooModule <span style="color:lightgray">#include "foo_file.c"</span>

FooModule brief description.

### internal void FooNew(); ![](https://img.shields.io/badge/-new_v1.0-green?style=plastic)

Brief description of this function.

More exhaustive description.

### internal void FooDeprecated(); ![](https://img.shields.io/badge/-deprecated-red?style=plastic)

Brief description of this function.

More exhaustive description.

!!! ERROR: Deprecated
    This function is deprecated, but a replacement was not provided. Please open an issue [here](${config.issuesUrl}).

## FooModule <span style="color:lightgray">#include "foo_file.c"</span>

FooModule brief description.

### internal void FooNew(); ![](https://img.shields.io/badge/-new_v1.0-green?style=plastic)

Brief description of this function.

More exhaustive description.

### internal void FooDeprecated(); ![](https://img.shields.io/badge/-deprecated-red?style=plastic)

Brief description of this function.

More exhaustive description.

!!! ERROR: Deprecated
    This function is deprecated, but a replacement was not provided. Please open an issue [here](${config.issuesUrl}).

### Demo

!!!
    I'm a note. Don't mind me, I'm just sitting here.

!!! note
    Another note.

!!! Tip
    Close the door on the way out.

!!! WARNING
    I'm a warning, perhaps. *Something might happen!*

!!! ERROR: Seriously
    Watch out, something **bad** could happen.

${endHtml}`;

  fs.writeFile(`${config.projectName}.docs.html`, htmlTemplate, { encoding: "utf8" });
})();


/**
 * 
 * @param {*} source 
 */
function parseSource(source) {
  for (const line of source.split("\n")) {
    if (line.indexOf("///") >= 0) {
      console.log(line);
    }
  }
}

/**
 * 
 * @param {string} dir [in|out] Directory to traverse.
 */
async function* getFiles(dir) {
  const dirents = await fs.readdir(dir, { withFileTypes: true });
  for (const dirent of dirents) {
    const res = path.resolve(dir, dirent.name);
    if (dirent.isDirectory()) {
      yield* getFiles(res);
    } else {
      yield res;
    }
  }
}
