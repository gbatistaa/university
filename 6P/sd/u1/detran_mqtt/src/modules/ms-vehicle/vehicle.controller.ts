import { Controller } from '@nestjs/common';
import { EventPattern, MessagePattern, Payload } from '@nestjs/microservices';
import { VehicleService } from './vehicle.service';
import { CreateVehicleDto } from './dto/create-vehicle.dto';
import { UpdateVehicleDto } from './dto/update-vehicle.dto';

@Controller()
export class VehicleController {
  constructor(private readonly vehicleService: VehicleService) {}

  @MessagePattern('commands/vehicle/signedThisYear')
  async getVehiclesSignedThisYear(@Payload() year: number) {
    return await this.vehicleService.getVehiclesSignedThisYear(year);
  }

  @EventPattern('commands/vehicle/calculateIPVA')
  calculateIPVA(@Payload() calculateIPVAPayload: { sign: string }) {
    return this.vehicleService.calculateIPVA(calculateIPVAPayload);
  }

  @MessagePattern('commands/vehicle/signup')
  async signupVehicle(@Payload() createVehicleDto: CreateVehicleDto) {
    return await this.vehicleService.signupVehicle(createVehicleDto);
  }

  @MessagePattern('commands/vehicle/updateConductor')
  async updateVehicleConductor(
    @Payload()
    updateVehiclePayload: {
      sign: string;
      updateVehicleDto: UpdateVehicleDto;
    },
  ) {
    return this.vehicleService.updateVehicleConductor(
      updateVehiclePayload.sign,
      updateVehiclePayload.updateVehicleDto,
    );
  }

  @MessagePattern('commands/vehicle/findOne')
  findOneVehicle(@Payload() payload: { sign: string }) {
    return this.vehicleService.findOneVehicle(payload.sign);
  }
}
