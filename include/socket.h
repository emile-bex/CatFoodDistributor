void setupSocket();

void loopSocket();

void sendMessage(String eventType, const StaticJsonDocument<64> &json);