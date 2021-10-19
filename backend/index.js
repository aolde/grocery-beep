require("dotenv").config();

const mqtt = require("mqtt");
const coop = require("./product-resolver/coop");
const openFoodFacts = require("./product-resolver/openFoodFacts");
const addProductToList = require("./shopping-list-service/google-keep");

const productResolvers = [coop, openFoodFacts];

console.log("Connecting to MQTT server...");

const client = mqtt.connect("mqtt://" + process.env.MQTT_HOST_IP);

const BARCODE_SCAN = "grocery-beep/barcode-scan";
const PRODUCT_ADD = "grocery-beep/product-add";
const PRODUCT_ERROR = "grocery-beep/product-error";
const MQTT_TOPICS = [BARCODE_SCAN, PRODUCT_ADD];

client.on("offline", () => console.log("MQTT server is offline."));
client.on("error", (err) => console.log("MQTT error:", err));

client.on("connect", function () {
    console.log(
        "Connected to MQTT server. Subscribing to topics.",
        MQTT_TOPICS
    );
    client.subscribe(MQTT_TOPICS);
});

client.on("message", async function (topic, message) {
    console.log("MQTT message received", topic, message.toString());

    if (topic === BARCODE_SCAN) {
        const barcode = message.toString();

        console.log("Resolving barcode: ", barcode);
        const product = await resolveProduct(barcode);

        if (product) {
            console.log("Resolved product: ", product.name);

            client.publish(PRODUCT_ADD, JSON.stringify(product));
        } else {
            console.log("Could not resolve a product with barcode ", barcode);

            client.publish(
                PRODUCT_ERROR,
                JSON.stringify({
                    message:
                        "Could not resolve a product with barcode " + barcode,
                    barcode,
                })
            );
        }
    }

    if (topic === PRODUCT_ADD) {
        const product = JSON.parse(message.toString());

        console.log(
            "Adding product to shopping list: ",
            product.name + " (" + product.category + ")"
        );

        await addProductToList(product.name, product.category);
    }
});

process.on("SIGINT", function () {
    client.end();
});

async function resolveProduct(barcode, resolverIndex = 0) {
    if (resolverIndex >= 0 && resolverIndex < productResolvers.length) {
        try {
            const product = await productResolvers[resolverIndex](barcode);
            console.log("Resolver ", resolverIndex, product);

            if (product === null) {
                return await resolveProduct(barcode, resolverIndex + 1);
            }
            return product;
        } catch (error) {
            console.log("Resolver threw error", resolverIndex, errror);
            return await resolveProduct(barcode, resolverIndex + 1);
        }
    } else {
        return null;
    }
}
