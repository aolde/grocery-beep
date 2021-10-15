require("dotenv").config();

const mqtt = require("mqtt");
const resolveProduct = require("./product-resolver/coop");
const addProductToList = require("./shopping-list-service/google-keep");

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
