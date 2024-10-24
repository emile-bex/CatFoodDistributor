#include <ArduinoJson.h>
#include <CustomJWT.h>
#include <SocketIOclient.h>
#include <socket.h>
#include <motor.h>
#include <constants.h>
#include <secrets.h>
#include <wifi.h>
#include "../.pio/libdeps/nodemcu/WebSockets/src/SocketIOclient.h"

char key[] = SECRET;

CustomJWT jwt(key, 256);

SocketIOclient socketIO;

void sendMessage(const StaticJsonDocument<64> &json) {
  StaticJsonDocument<64> doc;

  doc[0] = "event_name";
  doc[1] =json;

  String output = "";
  serializeJson(doc, output);

  socketIO.sendEVENT(output);

  Serial.println("Sent : " + output);
}

void onMessageCallback(String eventType, String message) {
  StaticJsonDocument<64> payload;

  deserializeJson(payload, message);

  if (eventType == IDENTIFICATION_SUCCESS) {
    Serial.println("Successfully identified");
  }

  if (eventType == IDENTIFICATION_ERROR) {
    Serial.println("Error while identifying");
  }

  if (eventType == SERVE_FOOD) {
    moveServo();

    StaticJsonDocument<64> foodServedConfirmationEvent;

    foodServedConfirmationEvent["event"] = FOOD_SERVED;

    sendMessage(foodServedConfirmationEvent);
  }
}

void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case sIOtype_DISCONNECT:
      Serial.printf("[IOc] Disconnected!\n");
      break;
    case sIOtype_CONNECT:
      Serial.printf("[IOc] Connected to url: %s\n", payload);

      // join default namespace (no auto join in Socket.IO V3)
      socketIO.send(sIOtype_CONNECT, "/");
      break;
    case sIOtype_EVENT: {
      char *sptr = NULL;
      int id = strtol((char *) payload, &sptr, 10);
      Serial.printf("[IOc] get event: %s id: %d\n", payload, id);
      if (id) {
        payload = (uint8_t *) sptr;
      }
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload, length);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
      }

      String eventName = doc[0];
      Serial.printf("[IOc] event name: %s\n", eventName.c_str());
      onMessageCallback(eventName, doc[1]);
      // Message Includes a ID for a ACK (callback)
      if (id) {
        // creat JSON message for Socket.IO (ack)
        DynamicJsonDocument docOut(1024);
        JsonArray array = docOut.to<JsonArray>();

        // add payload (parameters) for the ack (callback function)
        JsonObject param1 = array.createNestedObject();
        param1["now"] = millis();

        // JSON to String (serializion)
        String output;
        output += id;
        serializeJson(docOut, output);

        // Send event
        socketIO.send(sIOtype_ACK, output);
      }
    }
      break;
    case sIOtype_ACK:
      Serial.printf("[IOc] get ack: %u\n", length);
      break;
    case sIOtype_ERROR:
      Serial.printf("[IOc] get error: %u\n", length);
      break;
    case sIOtype_BINARY_EVENT:
      Serial.printf("[IOc] get binary: %u\n", length);
      break;
    case sIOtype_BINARY_ACK:
      Serial.printf("[IOc] get binary ack: %u\n", length);
      break;
  }
}


void setupSocket() {
  StaticJsonDocument<64> idJWT;
  idJWT["id"] = getMacAddress();

  String jwtObject = "";
  serializeJson(idJWT, jwtObject);

  jwt.allocateJWTMemory();

  int jwtLength  = jwtObject.length() + 1;
  char jwtBuffer[jwtLength];

  jwtObject.toCharArray(jwtBuffer,jwtLength);

  jwt.encodeJWT(jwtBuffer);

  Serial.printf("Header: %s\nHeader Length: %d\n", jwt.header, jwt.headerLength);
  Serial.printf("Payload: %s\nPayload Length: %d\n", jwt.payload, jwt.payloadLength);
  Serial.printf("Signature: %s\nSignature Length: %d\n", jwt.signature, jwt.signatureLength);
  Serial.printf("Final Output: %s\nFinalOutput Length: %d\n", jwt.out, jwt.outputLength);


/*  StaticJsonDocument<64> identificationEvent;

  identificationEvent["event"] = IDENTIFICATION;
  identificationEvent["data"] = jwt.payload;

  sendMessage(identificationEvent);*/

  const String port = SOCKET_PORT;

  // webSocket.beginSSL(SOCKET_URL, 81, "?token=" + jwt.out);
  socketIO.begin(SOCKET_URL, 3000, "/socket.io/?EIO=4&token=" + (String)jwt.out);
  socketIO.onEvent(socketIOEvent);
  socketIO.setReconnectInterval(5000);
}

void loopSocket() {
  socketIO.loop();
}
