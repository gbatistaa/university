import { Controller } from '@nestjs/common';
import { PenaltyService } from './penalty.service';

@Controller()
export class PenaltyController {
  constructor(private readonly penaltyService: PenaltyService) {}
}
