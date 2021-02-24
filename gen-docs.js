// @ts-check

/**
 * GenDocs default Config.
 */
const config = {
  srcDir: "./src",
  title: "Hyper | API Docs",
  lang: "en",
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
  for await (const f of getFiles(config.srcDir)) {
    files.push(f);
    const fileData = (await fs.readFile(f, { encoding: "utf8" })).split("\n");
    // const res = parseSource(fileData);
  }
  const res = parseSource(testSrc);

  const htmlTemplate = `
${startHtml}
  # Files

${files.map((filePath) => "- " + filePath).join("\n")}

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

  // fs.writeFile("docs.html", htmlTemplate, { encoding: "utf8" });

  // console.log(htmlTemplate);
})();


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
