const { default: got } = require("got");

const URL =
    "https://proxy.api.coop.se/external/personalization/search/products?api-version=v1&store=016001&groups=CUSTOMER_PRIVATE&direct=true";

const defaultHeaders = {
    "Content-Type": "application/json;charset=UTF-8",
    "Ocp-Apim-Subscription-Key": "c740b5c278fe417fba101fb6b15134b7",
};

async function resolveProduct(barcode) {
    const payload = {
        query: barcode,
        resultsOptions: { skip: 0, take: 1, sortBy: [], facets: [] },
        relatedResultsOptions: { skip: 0, take: 0 },
    };

    const response = await got.post(URL, {
        json: payload,
        responseType: "json",
        headers: defaultHeaders,
    });

    if (response.statusCode !== 200) {
        throw Error(
            "Could not resolve product from barcode " +
                barcode +
                " with resolver 'coop'. Status code: " +
                response.statusCode
        );
    }

    const productMatch = response.body.results.items?.[0];

    if (!productMatch) {
        return null;
    }

    return {
        name: productMatch.name,
        category: getRootCategory(productMatch.navCategories?.[0])?.name,
    };
}

function getRootCategory(category) {
    if (!category) {
        return null;
    }
    if (!category.superCategories || category.superCategories.length === 0) {
        return category;
    }
    return getRootCategory(category.superCategories[0]);
}

module.exports = resolveProduct;
