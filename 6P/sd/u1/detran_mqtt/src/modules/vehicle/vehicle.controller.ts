import { Controller } from '@nestjs/common';
import { MessagePattern, Payload } from '@nestjs/microservices';
import { VehicleService } from './vehicle.service';
import { CreateVehicleDto } from './dto/create-vehicle.dto';
import { UpdateVehicleDto } from './dto/update-vehicle.dto';

@Controller()
export class VehicleController {
  constructor(private readonly vehicleService: VehicleService) {}

  @MessagePattern('createVehicle')
  create(@Payload() createVehicleDto: CreateVehicleDto) {
    return this.vehicleService.create(createVehicleDto);
  }

  @MessagePattern('findAllVehicle')
  findAll() {
    return this.vehicleService.findAll();
  }

  @MessagePattern('findOneVehicle')
  findOne(@Payload() id: number) {
    return this.vehicleService.findOne(id);
  }

  @MessagePattern('updateVehicle')
  update(@Payload() updateVehicleDto: UpdateVehicleDto) {
    return this.vehicleService.update(updateVehicleDto.id, updateVehicleDto);
  }

  @MessagePattern('removeVehicle')
  remove(@Payload() id: number) {
    return this.vehicleService.remove(id);
  }
}
