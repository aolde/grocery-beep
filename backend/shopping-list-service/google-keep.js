const util = require("util");
const exec = util.promisify(require("child_process").exec);

const KEEP_NOTES_ID = process.env.GOOGLE_KEEP_NOTES_ID;

async function addProductToList(productName, productCategory) {
    const { error, stdout, stderr } = await exec(
        `gkeep items add ${KEEP_NOTES_ID} "${productName}  (${productCategory})" --uncheck`
    );

    if (error) {
        console.log(`gkeep error: ${error.message}`);
        return;
    }

    if (stderr) {
        console.log(`gkeep stderr: ${stderr}`);
        return;
    }

    // all good when stdout length is zero
    if (stdout.length === 0) {
        console.log(
            `Google Keep: Saved check item "${productName}" in note ${KEEP_NOTES_ID}.`
        );
    } else {
        console.log(`gkeep stdout: ${stdout.length}`);
    }
}

module.exports = addProductToList;
