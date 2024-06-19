package com.example.Communication.Server;

import lombok.RequiredArgsConstructor;
import org.springframework.http.HttpStatus;
import org.springframework.web.bind.annotation.*;
import org.springframework.http.ResponseEntity;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.logging.Logger;

@RestController
@RequestMapping("/sensors")
@RequiredArgsConstructor
@ResponseBody
public class SensorController {
    private static final List<String> AllowedPath = Arrays.asList("fire", "light", "gas", "temp", "false");
    private final SensorRepo sr;
    private final WebSocketController wc;
    private final static Logger LOG = Logger.getGlobal();


    @PostMapping("/{type}/save")
    public ResponseEntity<String> saveSensorData(@PathVariable("type") String sensorType, @RequestParam("value") String sensorValue, @RequestParam("state") String sensorState) {
        if (AllowedPath.contains(sensorType)) {
            SensorDTO tmp = SensorDTO.builder()
                    .name(sensorType)
                    .value(Double.parseDouble(sensorValue))
                    .state(Boolean.parseBoolean(sensorState))
                    .build();

            boolean saved = sr.saveSensorValue(tmp);
            if (saved) {
                LOG.info("Post 요청 - 센서 데이터 저장"+sensorType+"/"+sensorValue+"/"+sensorState);
                wc.sendSensorData(tmp);
                return ResponseEntity.ok("Sensor data saved successfully");
            }
            else {
                return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body("Failed to save sensor data");
            }
        }
        else {
            return ResponseEntity.status(HttpStatus.BAD_REQUEST).body("Wrong sensor type");
        }
    }

    @GetMapping("/{type}/retrieve")
    public ResponseEntity<List<SensorDTO>> retrieveSensorData(@PathVariable("type") String sensorType) {
        // 유효한 센서 타입만 처리
        if (AllowedPath.contains(sensorType)) {
            LOG.info("Get 요청 - "+sensorType+" 데이터 반환");
            //DB나 클래스 변수에 저장된 데이터 반환
            List<SensorDTO> tmp = sr.getSensorValue(sensorType);

            if (tmp != null) {
                return ResponseEntity.ok(tmp);
            }
            else {
                return ResponseEntity.status(HttpStatus.NOT_FOUND).body(Collections.emptyList());
            }
        }
        else {
            return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(Collections.emptyList());
        }
    }

    @GetMapping("/all/retrieve")
    public ResponseEntity<List<SensorDTO>> retrieveAllSensorData() {
        List<SensorDTO> tmp = sr.getAllSensorValue();
        LOG.info("Get 요청 - 모든 데이터 반환");
        if (tmp != null) {
            return ResponseEntity.ok(tmp);
        }
        else {
            return ResponseEntity.status(HttpStatus.NOT_FOUND).body(Collections.emptyList());
        }
    }
}
