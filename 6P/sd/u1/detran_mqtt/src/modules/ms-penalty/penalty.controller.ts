import { Controller } from '@nestjs/common';
import { EventPattern, Payload } from '@nestjs/microservices';
import { PenaltyService } from './penalty.service';
import { CreatePenaltyDto } from './dto/create-penalty.dto';

@Controller()
export class PenaltyController {
  constructor(private readonly penaltyService: PenaltyService) {}

  @EventPattern('commands/penalty/create')
  create(@Payload() createPenaltyDto: CreatePenaltyDto) {
    return this.penaltyService.createPenalty(createPenaltyDto);
  }

  @EventPattern('commands/penalty/vehiclePenalties')
  showVehiclePenalties(@Payload() vehicleData: { sign: string }) {
    return this.penaltyService.getVehiclePenalties(vehicleData.sign);
  }
}
