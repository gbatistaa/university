import { Inject, Injectable } from '@nestjs/common';
import { CreatePenaltyDto } from './dto/create-penalty.dto';
import { Penalty } from './entities/penalty.entity';
import { Raw, Repository } from 'typeorm';
import { ClientProxy } from '@nestjs/microservices';
import { InjectRepository } from '@nestjs/typeorm';
import { firstValueFrom } from 'rxjs';
import { Vehicle } from '../ms-vehicle/entities/vehicle.entity';

@Injectable()
export class PenaltyService {
  constructor(
    @Inject('MQTT_CLIENT') private readonly mqttClient: ClientProxy,
    @InjectRepository(Penalty) private repo: Repository<Penalty>,
  ) {}

  launchPenalty(createPenaltyDto: CreatePenaltyDto) {
    const penalty = this.repo.create(createPenaltyDto);
    this.mqttClient.emit('events/penalty/launch', penalty);

    return this.repo.save(penalty);
  }

  async getVehiclePenalties(vehiclePayload: { sign: string; year: string }) {
    const vehicle$ = this.mqttClient.send<Vehicle>('commands/vehicle/findOne', {
      sign: vehiclePayload.sign,
    });

    const vehicle: Vehicle = await firstValueFrom<Vehicle>(vehicle$);
    const penalties = await this.repo.find({
      where: {
        vehicleSign: vehicle.sign,
        createdAt: Raw((alias) => `strftime('%Y', ${alias}) = :year`, {
          year: vehiclePayload.year,
        }),
      },
    });

    this.mqttClient.emit('events/penalty/vehiclePenalties', {
      vehicleSign: vehicle.sign,
      year: vehiclePayload.year,
      penaltiesCount: penalties.length,
    });

    return penalties;
  }
}
