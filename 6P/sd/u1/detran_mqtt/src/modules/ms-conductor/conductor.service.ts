import { Inject, Injectable } from '@nestjs/common';
import { CreateConductorDto } from './dto/create-conductor.dto';
import { UpdateConductorDto } from './dto/update-conductor.dto';
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

  async create(createConductorDto: CreateConductorDto) {
    const createdConductor = this.repo.create(createConductorDto);
    await this.repo.save(createdConductor);

    // emissão de evento de registro de condutor:
    this.mqttClient.emit('createConductor', createdConductor);

    return createdConductor;
  }

  findOne(id: string) {
    return `This action returns a #${id} conductor`;
  }

  update(cpf: string, updateConductorDto: UpdateConductorDto) {
    return `This action updates a #${cpf} conductor`;
  }

  remove(id: string) {
    return `This action removes a #${id} conductor`;
  }
}
