import { Controller } from '@nestjs/common';
import { EventPattern, MessagePattern, Payload } from '@nestjs/microservices';
import { ConductorService } from './conductor.service';
import { CreateConductorDto } from './dto/create-conductor.dto';

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
  @MessagePattern('commands/conductor/findOne')
  findOne(@Payload() cpf: string) {
    return this.conductorService.findOneConductor(cpf);
  }
}
