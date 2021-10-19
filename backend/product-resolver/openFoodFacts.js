const { default: got } = require("got");

// Docs: https://world.openfoodfacts.org/data

const BASE_URL = "https://world.openfoodfacts.org/api/v0/";

const defaultHeaders = {
    "Content-Type": "application/json;charset=UTF-8",
    "User-Agent": "grocery-beep nodejs v1.0",
};

async function resolveProduct(barcode) {
    const url = `${BASE_URL}product/${barcode}.json`;

    const response = await got.get(url, {
        responseType: "json",
        headers: defaultHeaders,
    });

    if (response.statusCode !== 200) {
        throw Error(
            "Could not resolve product from barcode " +
                barcode +
                " with resolver 'openFoodFacts'. Status code: " +
                response.statusCode
        );
    }

    if (response.body.status === 0) {
        return null;
    }

    const productMatch = response.body.product;

    if (!productMatch) {
        return null;
    }

    return {
        name: productMatch.product_name,
        category: null, // there is a `categories` field but the values are long so we're ignoring it.
    };
}

module.exports = resolveProduct;
