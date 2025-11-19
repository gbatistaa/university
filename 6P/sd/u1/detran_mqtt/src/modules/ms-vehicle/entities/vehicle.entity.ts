import { CommonEntity } from 'src/modules/common/entities/common.entity';
import { Conductor } from 'src/modules/ms-conductor/entities/conductor.entity';
import { Column, Entity, JoinColumn, ManyToOne, Unique } from 'typeorm';

@Entity()
@Unique(['sign'])
export class Vehicle extends CommonEntity {
  @Column({ type: 'text', nullable: false, length: 11 })
  sign: string;

  @Column({ type: 'text', nullable: false })
  model: string;

  @Column({ type: 'float', nullable: false })
  value: number;

  @Column({ type: 'text', nullable: false, length: 11 })
  conductorCpf: string;

  @ManyToOne(() => Conductor, (conductor) => conductor.cpf)
  @JoinColumn({ name: 'conductorCpf', referencedColumnName: 'cpf' })
  conductor: Conductor;
}
