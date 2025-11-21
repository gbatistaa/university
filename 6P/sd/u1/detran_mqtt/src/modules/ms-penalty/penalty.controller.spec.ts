import { Test, TestingModule } from '@nestjs/testing';
import { PenaltyController } from './penalty.controller';
import { PenaltyService } from './penalty.service';

describe('PenaltyController', () => {
  let controller: PenaltyController;

  beforeEach(async () => {
    const module: TestingModule = await Test.createTestingModule({
      controllers: [PenaltyController],
      providers: [PenaltyService],
    }).compile();

    controller = module.get<PenaltyController>(PenaltyController);
  });

  it('should be defined', () => {
    expect(controller).toBeDefined();
  });
});
