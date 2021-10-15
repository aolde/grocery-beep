const util = require("util");
const exec = util.promisify(require("child_process").exec);

const KEEP_NOTES_ID = process.env.GOOGLE_KEEP_NOTES_ID;

async function addProductToList(productName, productCategory) {
    const { error, stdout, stderr } = await exec(
        `gkeep items add ${KEEP_NOTES_ID} "${productName}  (${productCategory})" --uncheck`
    );

    if (error) {
        console.log(`error: ${error.message}`);
        return;
    }

    if (stderr) {
        console.log(`stderr: ${stderr}`);
        return;
    }

    console.log(`stdout: ${stdout}`);
}

module.exports = addProductToList;
