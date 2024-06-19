package com.example.Communication.Server;

import lombok.*;
import java.time.LocalDateTime;

@NoArgsConstructor
@AllArgsConstructor
@Getter
@Setter
@Builder
public class SensorDTO {
    String name;
    double value;
    boolean state;
    LocalDateTime date;
}
