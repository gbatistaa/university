import { Controller } from '@nestjs/common';
import { EventPattern, Payload } from '@nestjs/microservices';
import { ConductorService } from './conductor.service';
import { CreateConductorDto } from './dto/create-conductor.dto';
import { UpdateConductorDto } from './dto/update-conductor.dto';

// O controller MQTT escuta os eventos:
@Controller()
export class ConductorController {
  constructor(private readonly conductorService: ConductorService) {}

  // registrar condutor
  @EventPattern('commands/conductor/signup')
  signupConductor(@Payload() createConductorDto: CreateConductorDto) {
    return this.conductorService.signupConductor(createConductorDto);
  }

  // buscar informações de um condutor
  @EventPattern('commands/conductor/findOne')
  findOne(@Payload() id: string) {
    return this.conductorService.findOne(id);
  }

  // atualizar condutor
  @EventPattern('commands/conductor/update')
  update(@Payload() updateConductorDto: UpdateConductorDto) {
    return this.conductorService.update(
      updateConductorDto.cpf,
      updateConductorDto,
    );
  }
}
