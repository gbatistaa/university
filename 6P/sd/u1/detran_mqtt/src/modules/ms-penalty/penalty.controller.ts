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

  @MessagePattern('commands/penalty/conductorPenalties')
  showConductorPenalties(
    @Payload() conductorPayload: { cpf: string; year: string },
  ) {
    return this.penaltyService.getConductorPenalties(conductorPayload);
  }

  @MessagePattern('commands/penalty/allPenalties')
  showAllPenalties(@Payload() payload: { year: string }) {
    return this.penaltyService.getAllPenalties(payload);
  }

  @MessagePattern('commands/penalty/topPenaltyScores')
  getTopConductorsByPenaltyScore(@Payload() payload: { limit: number }) {
    return this.penaltyService.getTopConductorsByPenaltyScore(payload);
  }
}
