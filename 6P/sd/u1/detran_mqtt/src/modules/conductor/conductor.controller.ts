import { Controller } from '@nestjs/common';
import { EventPattern, Payload } from '@nestjs/microservices';
import { ConductorService } from './conductor.service';
import { CreateConductorDto } from './dto/create-conductor.dto';
import { UpdateConductorDto } from './dto/update-conductor.dto';

@Controller()
export class ConductorController {
  constructor(private readonly conductorService: ConductorService) {}

  @EventPattern('createConductor')
  create(@Payload() createConductorDto: CreateConductorDto) {
    return this.conductorService.create(createConductorDto);
  }

  @EventPattern('findAllConductor')
  findAll() {
    return this.conductorService.findAll();
  }

  @EventPattern('findOneConductor')
  findOne(@Payload() id: number) {
    return this.conductorService.findOne(id);
  }

  @EventPattern('updateConductor')
  update(@Payload() updateConductorDto: UpdateConductorDto) {
    return this.conductorService.update(
      updateConductorDto.id,
      updateConductorDto,
    );
  }

  @EventPattern('removeConductor')
  remove(@Payload() id: number) {
    return this.conductorService.remove(id);
  }
}
