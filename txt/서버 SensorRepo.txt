package com.example.Communication.Server;

import lombok.RequiredArgsConstructor;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.time.format.DateTimeFormatter;
import java.util.List;

@Repository
@RequiredArgsConstructor
public class SensorRepo {
    private final JdbcTemplate jdbcTemplate;
    private static final DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
    public List<SensorDTO> getSensorValue(String sensorType){
        List<SensorDTO> list = jdbcTemplate.query(
                "select * from Sensor where type = ? order by date DESC",
                (rs, rowNum) -> {
                    SensorDTO sensor = new SensorDTO();
                    sensor.setName(rs.getString("type"));
                    sensor.setValue(rs.getDouble("value"));
                    sensor.setState(rs.getBoolean("state"));
                    sensor.setDate(rs.getTimestamp("date").toLocalDateTime());
                    return sensor;
                }, sensorType
        );
        return list;
    }

    public List<SensorDTO> getAllSensorValue(){
        List<SensorDTO> list = jdbcTemplate.query(
                "select * from Sensor order by date DESC",
                (rs, rowNum) -> {
                    SensorDTO sensor = new SensorDTO();
                    sensor.setName(rs.getString("type"));
                    sensor.setValue(rs.getDouble("value"));
                    sensor.setState(rs.getBoolean("state"));
                    sensor.setDate(rs.getTimestamp("date").toLocalDateTime());
                    return sensor;
                }
        );
        return list;
    }

    public boolean saveSensorValue(SensorDTO sensor) {
        if(sensor.getName().equals("false"))
            return true;
        int affectedRows = jdbcTemplate.update(
                "INSERT INTO Sensor (type, value, state) VALUES (?, ?, ?)",
                sensor.getName(),
                sensor.getValue(),
                sensor.isState()
        );
        return affectedRows > 0;
    }
}
