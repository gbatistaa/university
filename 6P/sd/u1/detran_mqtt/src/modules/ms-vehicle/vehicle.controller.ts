import { Controller } from '@nestjs/common';
import { EventPattern, Payload } from '@nestjs/microservices';
import { VehicleService } from './vehicle.service';
import { CreateVehicleDto } from './dto/create-vehicle.dto';
import { UpdateVehicleDto } from './dto/update-vehicle.dto';

@Controller()
export class VehicleController {
  constructor(private readonly vehicleService: VehicleService) {}

  @EventPattern('commands/vehicle/signup')
  async create(@Payload() createVehicleDto: CreateVehicleDto) {
    return await this.vehicleService.signup(createVehicleDto);
  }

  @EventPattern('commands/vehicle/updateConductor')
  update(@Payload() updateVehicleDto: UpdateVehicleDto) {
    return this.vehicleService.updateVehicleConductor(
      updateVehicleDto.sign as string,
      updateVehicleDto,
    );
  }

  @EventPattern('commands/vehicle/calculateIPVA')
  calculateIPVA(@Payload() calculateIPVADto: { sign: string }) {
    return this.vehicleService.calculateIPVA(calculateIPVADto);
  }
}
