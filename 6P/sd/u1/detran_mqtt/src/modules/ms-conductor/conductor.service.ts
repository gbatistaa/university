import { Inject, Injectable, NotFoundException } from '@nestjs/common';
import { CreateConductorDto } from './dto/create-conductor.dto';
import { ClientProxy } from '@nestjs/microservices';
import { InjectRepository } from '@nestjs/typeorm';
import { Conductor } from './entities/conductor.entity';
import { Repository } from 'typeorm';

@Injectable()
export class ConductorService {
  constructor(
    // o cliente mqtt:
    @Inject('MQTT_CLIENT') private readonly mqttClient: ClientProxy,
    @InjectRepository(Conductor) private repo: Repository<Conductor>,
  ) {}

  async signupConductor(createConductorDto: CreateConductorDto) {
    const createdConductor = this.repo.create(createConductorDto);
    await this.repo.save(createdConductor);

    // emissão de evento de registro de condutor:
    this.mqttClient.emit('event/conductor/signup', createdConductor);

    return createdConductor;
  }

  async findOneConductor(cpf: string) {
    const conductor = await this.repo.findOne({ where: { cpf } });

    if (!conductor) {
      throw new NotFoundException(`Conductor with CPF ${cpf} not found`);
    }

    return conductor;
  }

  async getTopConductorsByPenaltyScore(payload: {
    limit: number;
  }): Promise<
    Array<{ c_name: string; c_cpf: string; totalPontuation: string }>
  > {
    const topConductors = await this.repo
      .createQueryBuilder('c')
      .innerJoin('c.penalties', 'p')
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
