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
    console.log('achou');

    const penalties = await this.repo.find({
      where: {
        vehicleSign: vehicle.sign,
        createdAt: Raw((alias) => `strftime('%Y', ${alias}) = :year`, {
          year: vehiclePayload.year,
        }),
      },
      relations: ['conductor'],
    });

    this.mqttClient.emit('events/penalty/vehiclePenalties', {
      vehicleSign: vehiclePayload.sign,
      year: vehiclePayload.year,
      penalties,
    });

    return penalties;
  }

  async getConductorPenalties(conductorPayload: { cpf: string; year: string }) {
    const penalties = await this.repo.find({
      where: {
        conductorCpf: conductorPayload.cpf,
        createdAt: Raw((alias) => `strftime('%Y', ${alias}) = :year`, {
          year: conductorPayload.year,
        }),
      },
    });

    this.mqttClient.emit('events/penalty/conductorPenalties', {
      conductorCpf: conductorPayload.cpf,
      year: conductorPayload.year,
      penaltiesCount: penalties.length,
    });

    return penalties;
  }

  async getAllPenalties(payload: { year: string }) {
    const penalties = await this.repo.find({
      where: {
        createdAt: Raw((alias) => `strftime('%Y', ${alias}) = :year`, {
          year: payload.year,
        }),
      },
    });

    this.mqttClient.emit('events/penalty/allPenalties', {
      year: payload.year,
      penaltiesCount: penalties.length,
    });

    return penalties;
  }

  async getTopConductorsByPenaltyScore(payload: {
    limit: number;
  }): Promise<
    Array<{ c_name: string; c_cpf: string; totalPontuation: string }>
  > {
    const topConductors = await this.repo
      .createQueryBuilder('p')
      .innerJoin('p.conductor', 'c')
      .select(['c.name', 'c.cpf'])
      .addSelect('SUM(p.pontuation)', 'totalPontuation')
      .groupBy('c.cpf')
      .addGroupBy('c.name')
      .orderBy('totalPontuation', 'DESC')
      .limit(payload.limit)
      .getRawMany();

    return topConductors as Array<{
      c_name: string;
      c_cpf: string;
      totalPontuation: string;
    }>;
  }
}
