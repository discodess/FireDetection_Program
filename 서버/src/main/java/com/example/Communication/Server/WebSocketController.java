package com.example.Communication.Server;

import org.springframework.stereotype.Component;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.logging.Logger;

@Component
public class WebSocketController extends TextWebSocketHandler {
    private final static Logger LOG = Logger.getGlobal();
    private Map<String, WebSocketSession> sessions = new ConcurrentHashMap<>();

    //웹 소켓 최초 연결 후 작동
    @Override
    public void afterConnectionEstablished(WebSocketSession session) throws Exception {
        sessions.put(session.getId(), session);
        session.sendMessage(new TextMessage("Server Connected"));
    }
    //양방향 통신 로직
    @Override
    protected void handleTextMessage(WebSocketSession session, TextMessage message) throws Exception {
        LOG.info(message.getPayload());
    }

    @Override
    public void afterConnectionClosed(WebSocketSession session, CloseStatus status) throws Exception {
        LOG.info("연결 해제");
    }

    public void sendSensorData(SensorDTO data){
        TextMessage message = new TextMessage(String.valueOf(data.isState()));
        LOG.info(String.valueOf(data.isState()));
        for(WebSocketSession session : sessions.values()){
            if(session.isOpen()){
                try{
                    session.sendMessage(message);
                    LOG.info("데이터 전송 완료");
                }catch (Exception e){
                    LOG.warning("failed to send message : "+e.getMessage());
                }
            }
        }
    }
}
