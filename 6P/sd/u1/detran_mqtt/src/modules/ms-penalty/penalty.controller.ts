import { Controller } from '@nestjs/common';
import { MessagePattern, Payload } from '@nestjs/microservices';
import { PenaltyService } from './penalty.service';
import { CreatePenaltyDto } from './dto/create-penalty.dto';

@Controller()
export class PenaltyController {
  constructor(private readonly penaltyService: PenaltyService) {}

  @MessagePattern('commands/penalty/launch')
  launchPenalty(@Payload() createPenaltyDto: CreatePenaltyDto) {
    return this.penaltyService.launchPenalty(createPenaltyDto);
  }

  @MessagePattern('commands/penalty/vehiclePenalties')
  showVehiclePenalties(
    @Payload() vehiclePayload: { sign: string; year: string },
  ) {
    return this.penaltyService.getVehiclePenalties(vehiclePayload);
  }
}
