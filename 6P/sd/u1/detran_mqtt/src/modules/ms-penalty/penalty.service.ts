import { Injectable } from '@nestjs/common';
import { CreatePenaltyDto } from './dto/create-penalty.dto';
import { UpdatePenaltyDto } from './dto/update-penalty.dto';

@Injectable()
export class PenaltyService {
  create(createPenaltyDto: CreatePenaltyDto) {
    return 'This action adds a new penalty';
  }

  findAll() {
    return `This action returns all penalty`;
  }

  findOne(id: number) {
    return `This action returns a #${id} penalty`;
  }

  update(id: number, updatePenaltyDto: UpdatePenaltyDto) {
    return `This action updates a #${id} penalty`;
  }

  remove(id: number) {
    return `This action removes a #${id} penalty`;
  }
}
