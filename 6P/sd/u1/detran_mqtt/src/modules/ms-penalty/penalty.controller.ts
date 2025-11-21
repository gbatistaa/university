import { Controller } from '@nestjs/common';
import { MessagePattern, Payload } from '@nestjs/microservices';
import { PenaltyService } from './penalty.service';
import { CreatePenaltyDto } from './dto/create-penalty.dto';
import { UpdatePenaltyDto } from './dto/update-penalty.dto';

@Controller()
export class PenaltyController {
  constructor(private readonly penaltyService: PenaltyService) {}

  @MessagePattern('createPenalty')
  create(@Payload() createPenaltyDto: CreatePenaltyDto) {
    return this.penaltyService.create(createPenaltyDto);
  }

  @MessagePattern('findAllPenalty')
  findAll() {
    return this.penaltyService.findAll();
  }

  @MessagePattern('findOnePenalty')
  findOne(@Payload() id: number) {
    return this.penaltyService.findOne(id);
  }

  @MessagePattern('updatePenalty')
  update(@Payload() updatePenaltyDto: UpdatePenaltyDto) {
    return this.penaltyService.update(updatePenaltyDto.id, updatePenaltyDto);
  }

  @MessagePattern('removePenalty')
  remove(@Payload() id: number) {
    return this.penaltyService.remove(id);
  }
}
