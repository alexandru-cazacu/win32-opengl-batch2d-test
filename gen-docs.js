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

(async () => {
  const files = [];
  for await (const f of getFiles(config.srcDir)) {
    files.push(f);
  }

  const htmlTemplate = `
<meta charset="utf-8" lang="${config.lang}"><style class="fallback">body{visibility:hidden;}</style>

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

<!-- Markdeep: -->
<head><title>${config.title}</title></head>
<link rel="stylesheet" href="https://casual-effects.com/markdeep/latest/apidoc.css">
<script src="https://morgan3d.github.io/markdeep/latest/markdeep.min.js"></script>`;

  fs.writeFile("docs.html", htmlTemplate, { encoding: 'utf8' });

  console.log(htmlTemplate);
})();

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
